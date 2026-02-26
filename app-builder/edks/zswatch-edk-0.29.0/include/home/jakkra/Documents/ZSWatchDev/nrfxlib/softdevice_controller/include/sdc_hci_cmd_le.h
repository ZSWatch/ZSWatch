/*
 * Copyright (c) Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sdc_hci_cmd_le.h
 *
 * @defgroup sdc_hci_le SoftDevice Controller HCI Interface for LE Controller commands
 * @ingroup sdc_hci
 *
 * All APIs in this header file are expected to be called from the
 * same execution priority as mpsl_low_priority_process.
 * Not doing so will lead to undefined behavior.
 *
 * @{
 */

#ifndef SDC_HCI_CMD_LE_H__
#define SDC_HCI_CMD_LE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nrfx.h>

/**
 * @defgroup HCI_TYPES Types
 * @{
 */

/** @brief HCI LE OpCode Field values. */
enum sdc_hci_opcode_le
{
    /** @brief See @ref sdc_hci_cmd_le_set_event_mask(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EVENT_MASK = 0x2001,
    /** @brief See @ref sdc_hci_cmd_le_read_buffer_size(). */
    SDC_HCI_OPCODE_CMD_LE_READ_BUFFER_SIZE = 0x2002,
    /** @brief See @ref sdc_hci_cmd_le_read_local_supported_features(). */
    SDC_HCI_OPCODE_CMD_LE_READ_LOCAL_SUPPORTED_FEATURES = 0x2003,
    /** @brief See @ref sdc_hci_cmd_le_set_random_address(). */
    SDC_HCI_OPCODE_CMD_LE_SET_RANDOM_ADDRESS = 0x2005,
    /** @brief See @ref sdc_hci_cmd_le_set_adv_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_ADV_PARAMS = 0x2006,
    /** @brief See @ref sdc_hci_cmd_le_read_adv_physical_channel_tx_power(). */
    SDC_HCI_OPCODE_CMD_LE_READ_ADV_PHYSICAL_CHANNEL_TX_POWER = 0x2007,
    /** @brief See @ref sdc_hci_cmd_le_set_adv_data(). */
    SDC_HCI_OPCODE_CMD_LE_SET_ADV_DATA = 0x2008,
    /** @brief See @ref sdc_hci_cmd_le_set_scan_response_data(). */
    SDC_HCI_OPCODE_CMD_LE_SET_SCAN_RESPONSE_DATA = 0x2009,
    /** @brief See @ref sdc_hci_cmd_le_set_adv_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_ADV_ENABLE = 0x200a,
    /** @brief See @ref sdc_hci_cmd_le_set_scan_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_SCAN_PARAMS = 0x200b,
    /** @brief See @ref sdc_hci_cmd_le_set_scan_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_SCAN_ENABLE = 0x200c,
    /** @brief See @ref sdc_hci_cmd_le_create_conn(). */
    SDC_HCI_OPCODE_CMD_LE_CREATE_CONN = 0x200d,
    /** @brief See @ref sdc_hci_cmd_le_create_conn_cancel(). */
    SDC_HCI_OPCODE_CMD_LE_CREATE_CONN_CANCEL = 0x200e,
    /** @brief See @ref sdc_hci_cmd_le_read_filter_accept_list_size(). */
    SDC_HCI_OPCODE_CMD_LE_READ_FILTER_ACCEPT_LIST_SIZE = 0x200f,
    /** @brief See @ref sdc_hci_cmd_le_clear_filter_accept_list(). */
    SDC_HCI_OPCODE_CMD_LE_CLEAR_FILTER_ACCEPT_LIST = 0x2010,
    /** @brief See @ref sdc_hci_cmd_le_add_device_to_filter_accept_list(). */
    SDC_HCI_OPCODE_CMD_LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST = 0x2011,
    /** @brief See @ref sdc_hci_cmd_le_remove_device_from_filter_accept_list(). */
    SDC_HCI_OPCODE_CMD_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST = 0x2012,
    /** @brief See @ref sdc_hci_cmd_le_conn_update(). */
    SDC_HCI_OPCODE_CMD_LE_CONN_UPDATE = 0x2013,
    /** @brief See @ref sdc_hci_cmd_le_set_host_channel_classification(). */
    SDC_HCI_OPCODE_CMD_LE_SET_HOST_CHANNEL_CLASSIFICATION = 0x2014,
    /** @brief See @ref sdc_hci_cmd_le_read_channel_map(). */
    SDC_HCI_OPCODE_CMD_LE_READ_CHANNEL_MAP = 0x2015,
    /** @brief See @ref sdc_hci_cmd_le_read_remote_features(). */
    SDC_HCI_OPCODE_CMD_LE_READ_REMOTE_FEATURES = 0x2016,
    /** @brief See @ref sdc_hci_cmd_le_encrypt(). */
    SDC_HCI_OPCODE_CMD_LE_ENCRYPT = 0x2017,
    /** @brief See @ref sdc_hci_cmd_le_rand(). */
    SDC_HCI_OPCODE_CMD_LE_RAND = 0x2018,
    /** @brief See @ref sdc_hci_cmd_le_enable_encryption(). */
    SDC_HCI_OPCODE_CMD_LE_ENABLE_ENCRYPTION = 0x2019,
    /** @brief See @ref sdc_hci_cmd_le_long_term_key_request_reply(). */
    SDC_HCI_OPCODE_CMD_LE_LONG_TERM_KEY_REQUEST_REPLY = 0x201a,
    /** @brief See @ref sdc_hci_cmd_le_long_term_key_request_negative_reply(). */
    SDC_HCI_OPCODE_CMD_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY = 0x201b,
    /** @brief See @ref sdc_hci_cmd_le_read_supported_states(). */
    SDC_HCI_OPCODE_CMD_LE_READ_SUPPORTED_STATES = 0x201c,
    /** @brief See @ref sdc_hci_cmd_le_receiver_test_v1(). */
    SDC_HCI_OPCODE_CMD_LE_RECEIVER_TEST_V1 = 0x201d,
    /** @brief See @ref sdc_hci_cmd_le_transmitter_test_v1(). */
    SDC_HCI_OPCODE_CMD_LE_TRANSMITTER_TEST_V1 = 0x201e,
    /** @brief See @ref sdc_hci_cmd_le_test_end(). */
    SDC_HCI_OPCODE_CMD_LE_TEST_END = 0x201f,
    /** @brief See @ref sdc_hci_cmd_le_set_data_length(). */
    SDC_HCI_OPCODE_CMD_LE_SET_DATA_LENGTH = 0x2022,
    /** @brief See @ref sdc_hci_cmd_le_read_suggested_default_data_length(). */
    SDC_HCI_OPCODE_CMD_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH = 0x2023,
    /** @brief See @ref sdc_hci_cmd_le_write_suggested_default_data_length(). */
    SDC_HCI_OPCODE_CMD_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH = 0x2024,
    /** @brief See @ref sdc_hci_cmd_le_add_device_to_resolving_list(). */
    SDC_HCI_OPCODE_CMD_LE_ADD_DEVICE_TO_RESOLVING_LIST = 0x2027,
    /** @brief See @ref sdc_hci_cmd_le_remove_device_from_resolving_list(). */
    SDC_HCI_OPCODE_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST = 0x2028,
    /** @brief See @ref sdc_hci_cmd_le_clear_resolving_list(). */
    SDC_HCI_OPCODE_CMD_LE_CLEAR_RESOLVING_LIST = 0x2029,
    /** @brief See @ref sdc_hci_cmd_le_read_resolving_list_size(). */
    SDC_HCI_OPCODE_CMD_LE_READ_RESOLVING_LIST_SIZE = 0x202a,
    /** @brief See @ref sdc_hci_cmd_le_set_address_resolution_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_ADDRESS_RESOLUTION_ENABLE = 0x202d,
    /** @brief See @ref sdc_hci_cmd_le_set_resolvable_private_address_timeout(). */
    SDC_HCI_OPCODE_CMD_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT = 0x202e,
    /** @brief See @ref sdc_hci_cmd_le_read_max_data_length(). */
    SDC_HCI_OPCODE_CMD_LE_READ_MAX_DATA_LENGTH = 0x202f,
    /** @brief See @ref sdc_hci_cmd_le_read_phy(). */
    SDC_HCI_OPCODE_CMD_LE_READ_PHY = 0x2030,
    /** @brief See @ref sdc_hci_cmd_le_set_default_phy(). */
    SDC_HCI_OPCODE_CMD_LE_SET_DEFAULT_PHY = 0x2031,
    /** @brief See @ref sdc_hci_cmd_le_set_phy(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PHY = 0x2032,
    /** @brief See @ref sdc_hci_cmd_le_receiver_test_v2(). */
    SDC_HCI_OPCODE_CMD_LE_RECEIVER_TEST_V2 = 0x2033,
    /** @brief See @ref sdc_hci_cmd_le_transmitter_test_v2(). */
    SDC_HCI_OPCODE_CMD_LE_TRANSMITTER_TEST_V2 = 0x2034,
    /** @brief See @ref sdc_hci_cmd_le_set_adv_set_random_address(). */
    SDC_HCI_OPCODE_CMD_LE_SET_ADV_SET_RANDOM_ADDRESS = 0x2035,
    /** @brief See @ref sdc_hci_cmd_le_set_ext_adv_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EXT_ADV_PARAMS = 0x2036,
    /** @brief See @ref sdc_hci_cmd_le_set_ext_adv_data(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EXT_ADV_DATA = 0x2037,
    /** @brief See @ref sdc_hci_cmd_le_set_ext_scan_response_data(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EXT_SCAN_RESPONSE_DATA = 0x2038,
    /** @brief See @ref sdc_hci_cmd_le_set_ext_adv_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EXT_ADV_ENABLE = 0x2039,
    /** @brief See @ref sdc_hci_cmd_le_read_max_adv_data_length(). */
    SDC_HCI_OPCODE_CMD_LE_READ_MAX_ADV_DATA_LENGTH = 0x203a,
    /** @brief See @ref sdc_hci_cmd_le_read_number_of_supported_adv_sets(). */
    SDC_HCI_OPCODE_CMD_LE_READ_NUMBER_OF_SUPPORTED_ADV_SETS = 0x203b,
    /** @brief See @ref sdc_hci_cmd_le_remove_adv_set(). */
    SDC_HCI_OPCODE_CMD_LE_REMOVE_ADV_SET = 0x203c,
    /** @brief See @ref sdc_hci_cmd_le_clear_adv_sets(). */
    SDC_HCI_OPCODE_CMD_LE_CLEAR_ADV_SETS = 0x203d,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_PARAMS = 0x203e,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_data(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_DATA = 0x203f,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_ENABLE = 0x2040,
    /** @brief See @ref sdc_hci_cmd_le_set_ext_scan_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EXT_SCAN_PARAMS = 0x2041,
    /** @brief See @ref sdc_hci_cmd_le_set_ext_scan_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EXT_SCAN_ENABLE = 0x2042,
    /** @brief See @ref sdc_hci_cmd_le_ext_create_conn(). */
    SDC_HCI_OPCODE_CMD_LE_EXT_CREATE_CONN = 0x2043,
    /** @brief See @ref sdc_hci_cmd_le_periodic_adv_create_sync(). */
    SDC_HCI_OPCODE_CMD_LE_PERIODIC_ADV_CREATE_SYNC = 0x2044,
    /** @brief See @ref sdc_hci_cmd_le_periodic_adv_create_sync_cancel(). */
    SDC_HCI_OPCODE_CMD_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL = 0x2045,
    /** @brief See @ref sdc_hci_cmd_le_periodic_adv_terminate_sync(). */
    SDC_HCI_OPCODE_CMD_LE_PERIODIC_ADV_TERMINATE_SYNC = 0x2046,
    /** @brief See @ref sdc_hci_cmd_le_add_device_to_periodic_adv_list(). */
    SDC_HCI_OPCODE_CMD_LE_ADD_DEVICE_TO_PERIODIC_ADV_LIST = 0x2047,
    /** @brief See @ref sdc_hci_cmd_le_remove_device_from_periodic_adv_list(). */
    SDC_HCI_OPCODE_CMD_LE_REMOVE_DEVICE_FROM_PERIODIC_ADV_LIST = 0x2048,
    /** @brief See @ref sdc_hci_cmd_le_clear_periodic_adv_list(). */
    SDC_HCI_OPCODE_CMD_LE_CLEAR_PERIODIC_ADV_LIST = 0x2049,
    /** @brief See @ref sdc_hci_cmd_le_read_periodic_adv_list_size(). */
    SDC_HCI_OPCODE_CMD_LE_READ_PERIODIC_ADV_LIST_SIZE = 0x204a,
    /** @brief See @ref sdc_hci_cmd_le_read_transmit_power(). */
    SDC_HCI_OPCODE_CMD_LE_READ_TRANSMIT_POWER = 0x204b,
    /** @brief See @ref sdc_hci_cmd_le_read_rf_path_compensation(). */
    SDC_HCI_OPCODE_CMD_LE_READ_RF_PATH_COMPENSATION = 0x204c,
    /** @brief See @ref sdc_hci_cmd_le_write_rf_path_compensation(). */
    SDC_HCI_OPCODE_CMD_LE_WRITE_RF_PATH_COMPENSATION = 0x204d,
    /** @brief See @ref sdc_hci_cmd_le_set_privacy_mode(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PRIVACY_MODE = 0x204e,
    /** @brief See @ref sdc_hci_cmd_le_receiver_test_v3(). */
    SDC_HCI_OPCODE_CMD_LE_RECEIVER_TEST_V3 = 0x204f,
    /** @brief See @ref sdc_hci_cmd_le_transmitter_test_v3(). */
    SDC_HCI_OPCODE_CMD_LE_TRANSMITTER_TEST_V3 = 0x2050,
    /** @brief See @ref sdc_hci_cmd_le_set_connless_cte_transmit_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_CONNLESS_CTE_TRANSMIT_PARAMS = 0x2051,
    /** @brief See @ref sdc_hci_cmd_le_set_connless_cte_transmit_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_CONNLESS_CTE_TRANSMIT_ENABLE = 0x2052,
    /** @brief See @ref sdc_hci_cmd_le_set_conn_cte_transmit_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_CONN_CTE_TRANSMIT_PARAMS = 0x2055,
    /** @brief See @ref sdc_hci_cmd_le_conn_cte_response_enable(). */
    SDC_HCI_OPCODE_CMD_LE_CONN_CTE_RESPONSE_ENABLE = 0x2057,
    /** @brief See @ref sdc_hci_cmd_le_read_antenna_information(). */
    SDC_HCI_OPCODE_CMD_LE_READ_ANTENNA_INFORMATION = 0x2058,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_receive_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_RECEIVE_ENABLE = 0x2059,
    /** @brief See @ref sdc_hci_cmd_le_periodic_adv_sync_transfer(). */
    SDC_HCI_OPCODE_CMD_LE_PERIODIC_ADV_SYNC_TRANSFER = 0x205a,
    /** @brief See @ref sdc_hci_cmd_le_periodic_adv_set_info_transfer(). */
    SDC_HCI_OPCODE_CMD_LE_PERIODIC_ADV_SET_INFO_TRANSFER = 0x205b,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_sync_transfer_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_SYNC_TRANSFER_PARAMS = 0x205c,
    /** @brief See @ref sdc_hci_cmd_le_set_default_periodic_adv_sync_transfer_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_DEFAULT_PERIODIC_ADV_SYNC_TRANSFER_PARAMS = 0x205d,
    /** @brief See @ref sdc_hci_cmd_le_read_buffer_size_v2(). */
    SDC_HCI_OPCODE_CMD_LE_READ_BUFFER_SIZE_V2 = 0x2060,
    /** @brief See @ref sdc_hci_cmd_le_read_iso_tx_sync(). */
    SDC_HCI_OPCODE_CMD_LE_READ_ISO_TX_SYNC = 0x2061,
    /** @brief See @ref sdc_hci_cmd_le_set_cig_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_CIG_PARAMS = 0x2062,
    /** @brief See @ref sdc_hci_cmd_le_set_cig_params_test(). */
    SDC_HCI_OPCODE_CMD_LE_SET_CIG_PARAMS_TEST = 0x2063,
    /** @brief See @ref sdc_hci_cmd_le_create_cis(). */
    SDC_HCI_OPCODE_CMD_LE_CREATE_CIS = 0x2064,
    /** @brief See @ref sdc_hci_cmd_le_remove_cig(). */
    SDC_HCI_OPCODE_CMD_LE_REMOVE_CIG = 0x2065,
    /** @brief See @ref sdc_hci_cmd_le_accept_cis_request(). */
    SDC_HCI_OPCODE_CMD_LE_ACCEPT_CIS_REQUEST = 0x2066,
    /** @brief See @ref sdc_hci_cmd_le_reject_cis_request(). */
    SDC_HCI_OPCODE_CMD_LE_REJECT_CIS_REQUEST = 0x2067,
    /** @brief See @ref sdc_hci_cmd_le_create_big(). */
    SDC_HCI_OPCODE_CMD_LE_CREATE_BIG = 0x2068,
    /** @brief See @ref sdc_hci_cmd_le_create_big_test(). */
    SDC_HCI_OPCODE_CMD_LE_CREATE_BIG_TEST = 0x2069,
    /** @brief See @ref sdc_hci_cmd_le_terminate_big(). */
    SDC_HCI_OPCODE_CMD_LE_TERMINATE_BIG = 0x206a,
    /** @brief See @ref sdc_hci_cmd_le_big_create_sync(). */
    SDC_HCI_OPCODE_CMD_LE_BIG_CREATE_SYNC = 0x206b,
    /** @brief See @ref sdc_hci_cmd_le_big_terminate_sync(). */
    SDC_HCI_OPCODE_CMD_LE_BIG_TERMINATE_SYNC = 0x206c,
    /** @brief See @ref sdc_hci_cmd_le_request_peer_sca(). */
    SDC_HCI_OPCODE_CMD_LE_REQUEST_PEER_SCA = 0x206d,
    /** @brief See @ref sdc_hci_cmd_le_setup_iso_data_path(). */
    SDC_HCI_OPCODE_CMD_LE_SETUP_ISO_DATA_PATH = 0x206e,
    /** @brief See @ref sdc_hci_cmd_le_remove_iso_data_path(). */
    SDC_HCI_OPCODE_CMD_LE_REMOVE_ISO_DATA_PATH = 0x206f,
    /** @brief See @ref sdc_hci_cmd_le_iso_transmit_test(). */
    SDC_HCI_OPCODE_CMD_LE_ISO_TRANSMIT_TEST = 0x2070,
    /** @brief See @ref sdc_hci_cmd_le_iso_receive_test(). */
    SDC_HCI_OPCODE_CMD_LE_ISO_RECEIVE_TEST = 0x2071,
    /** @brief See @ref sdc_hci_cmd_le_iso_read_test_counters(). */
    SDC_HCI_OPCODE_CMD_LE_ISO_READ_TEST_COUNTERS = 0x2072,
    /** @brief See @ref sdc_hci_cmd_le_iso_test_end(). */
    SDC_HCI_OPCODE_CMD_LE_ISO_TEST_END = 0x2073,
    /** @brief See @ref sdc_hci_cmd_le_set_host_feature(). */
    SDC_HCI_OPCODE_CMD_LE_SET_HOST_FEATURE = 0x2074,
    /** @brief See @ref sdc_hci_cmd_le_read_iso_link_quality(). */
    SDC_HCI_OPCODE_CMD_LE_READ_ISO_LINK_QUALITY = 0x2075,
    /** @brief See @ref sdc_hci_cmd_le_enhanced_read_transmit_power_level(). */
    SDC_HCI_OPCODE_CMD_LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL = 0x2076,
    /** @brief See @ref sdc_hci_cmd_le_read_remote_transmit_power_level(). */
    SDC_HCI_OPCODE_CMD_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL = 0x2077,
    /** @brief See @ref sdc_hci_cmd_le_set_path_loss_reporting_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PATH_LOSS_REPORTING_PARAMS = 0x2078,
    /** @brief See @ref sdc_hci_cmd_le_set_path_loss_reporting_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PATH_LOSS_REPORTING_ENABLE = 0x2079,
    /** @brief See @ref sdc_hci_cmd_le_set_transmit_power_reporting_enable(). */
    SDC_HCI_OPCODE_CMD_LE_SET_TRANSMIT_POWER_REPORTING_ENABLE = 0x207a,
    /** @brief See @ref sdc_hci_cmd_le_transmitter_test_v4(). */
    SDC_HCI_OPCODE_CMD_LE_TRANSMITTER_TEST_V4 = 0x207b,
    /** @brief See @ref sdc_hci_cmd_le_set_data_related_address_changes(). */
    SDC_HCI_OPCODE_CMD_LE_SET_DATA_RELATED_ADDRESS_CHANGES = 0x207c,
    /** @brief See @ref sdc_hci_cmd_le_set_default_subrate(). */
    SDC_HCI_OPCODE_CMD_LE_SET_DEFAULT_SUBRATE = 0x207d,
    /** @brief See @ref sdc_hci_cmd_le_subrate_request(). */
    SDC_HCI_OPCODE_CMD_LE_SUBRATE_REQUEST = 0x207e,
    /** @brief See @ref sdc_hci_cmd_le_set_ext_adv_params_v2(). */
    SDC_HCI_OPCODE_CMD_LE_SET_EXT_ADV_PARAMS_V2 = 0x207f,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_subevent_data(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_SUBEVENT_DATA = 0x2082,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_response_data(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_RESPONSE_DATA = 0x2083,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_sync_subevent(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_SYNC_SUBEVENT = 0x2084,
    /** @brief See @ref sdc_hci_cmd_le_ext_create_conn_v2(). */
    SDC_HCI_OPCODE_CMD_LE_EXT_CREATE_CONN_V2 = 0x2085,
    /** @brief See @ref sdc_hci_cmd_le_set_periodic_adv_params_v2(). */
    SDC_HCI_OPCODE_CMD_LE_SET_PERIODIC_ADV_PARAMS_V2 = 0x2086,
    /** @brief See @ref sdc_hci_cmd_le_read_all_local_supported_features(). */
    SDC_HCI_OPCODE_CMD_LE_READ_ALL_LOCAL_SUPPORTED_FEATURES = 0x2087,
    /** @brief See @ref sdc_hci_cmd_le_read_all_remote_features(). */
    SDC_HCI_OPCODE_CMD_LE_READ_ALL_REMOTE_FEATURES = 0x2088,
    /** @brief See @ref sdc_hci_cmd_le_cs_read_local_supported_capabilities(). */
    SDC_HCI_OPCODE_CMD_LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES = 0x2089,
    /** @brief See @ref sdc_hci_cmd_le_cs_read_remote_supported_capabilities(). */
    SDC_HCI_OPCODE_CMD_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES = 0x208a,
    /** @brief See @ref sdc_hci_cmd_le_cs_write_cached_remote_supported_capabilities(). */
    SDC_HCI_OPCODE_CMD_LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES = 0x208b,
    /** @brief See @ref sdc_hci_cmd_le_cs_security_enable(). */
    SDC_HCI_OPCODE_CMD_LE_CS_SECURITY_ENABLE = 0x208c,
    /** @brief See @ref sdc_hci_cmd_le_cs_set_default_settings(). */
    SDC_HCI_OPCODE_CMD_LE_CS_SET_DEFAULT_SETTINGS = 0x208d,
    /** @brief See @ref sdc_hci_cmd_le_cs_read_remote_fae_table(). */
    SDC_HCI_OPCODE_CMD_LE_CS_READ_REMOTE_FAE_TABLE = 0x208e,
    /** @brief See @ref sdc_hci_cmd_le_cs_write_cached_remote_fae_table(). */
    SDC_HCI_OPCODE_CMD_LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE = 0x208f,
    /** @brief See @ref sdc_hci_cmd_le_cs_create_config(). */
    SDC_HCI_OPCODE_CMD_LE_CS_CREATE_CONFIG = 0x2090,
    /** @brief See @ref sdc_hci_cmd_le_cs_remove_config(). */
    SDC_HCI_OPCODE_CMD_LE_CS_REMOVE_CONFIG = 0x2091,
    /** @brief See @ref sdc_hci_cmd_le_cs_set_channel_classification(). */
    SDC_HCI_OPCODE_CMD_LE_CS_SET_CHANNEL_CLASSIFICATION = 0x2092,
    /** @brief See @ref sdc_hci_cmd_le_cs_set_procedure_params(). */
    SDC_HCI_OPCODE_CMD_LE_CS_SET_PROCEDURE_PARAMS = 0x2093,
    /** @brief See @ref sdc_hci_cmd_le_cs_procedure_enable(). */
    SDC_HCI_OPCODE_CMD_LE_CS_PROCEDURE_ENABLE = 0x2094,
    /** @brief See @ref sdc_hci_cmd_le_cs_test(). */
    SDC_HCI_OPCODE_CMD_LE_CS_TEST = 0x2095,
    /** @brief See @ref sdc_hci_cmd_le_cs_test_end(). */
    SDC_HCI_OPCODE_CMD_LE_CS_TEST_END = 0x2096,
    /** @brief See @ref sdc_hci_cmd_le_set_host_feature_v2(). */
    SDC_HCI_OPCODE_CMD_LE_SET_HOST_FEATURE_V2 = 0x2097,
    /** @brief See @ref sdc_hci_cmd_le_frame_space_update(). */
    SDC_HCI_OPCODE_CMD_LE_FRAME_SPACE_UPDATE = 0x209d,
    /** @brief See @ref sdc_hci_cmd_le_conn_rate_request(). */
    SDC_HCI_OPCODE_CMD_LE_CONN_RATE_REQUEST = 0x20a1,
    /** @brief See @ref sdc_hci_cmd_le_set_default_rate_params(). */
    SDC_HCI_OPCODE_CMD_LE_SET_DEFAULT_RATE_PARAMS = 0x20a2,
    /** @brief See @ref sdc_hci_cmd_le_read_min_supported_conn_interval(). */
    SDC_HCI_OPCODE_CMD_LE_READ_MIN_SUPPORTED_CONN_INTERVAL = 0x20a3,
};

/** @brief LE subevent Code values. */
enum sdc_hci_subevent_le
{
    /** @brief See @ref sdc_hci_subevent_le_adv_report_t. */
    SDC_HCI_SUBEVENT_LE_ADV_REPORT = 0x02,
    /** @brief See @ref sdc_hci_subevent_le_directed_adv_report_t. */
    SDC_HCI_SUBEVENT_LE_DIRECTED_ADV_REPORT = 0x0b,
    /** @brief See @ref sdc_hci_subevent_le_ext_adv_report_t. */
    SDC_HCI_SUBEVENT_LE_EXT_ADV_REPORT = 0x0d,
    /** @brief See @ref sdc_hci_subevent_le_cis_established_t. */
    SDC_HCI_SUBEVENT_LE_CIS_ESTABLISHED = 0x19,
    /** @brief See @ref sdc_hci_subevent_le_subrate_change_t. */
    SDC_HCI_SUBEVENT_LE_SUBRATE_CHANGE = 0x23,
    /** @brief See @ref sdc_hci_subevent_le_cis_established_v2_t. */
    SDC_HCI_SUBEVENT_LE_CIS_ESTABLISHED_V2 = 0x2a,
    /** @brief See @ref sdc_hci_subevent_le_read_all_remote_features_complete_t. */
    SDC_HCI_SUBEVENT_LE_READ_ALL_REMOTE_FEATURES_COMPLETE = 0x2b,
    /** @brief See @ref sdc_hci_subevent_le_cs_read_remote_supported_capabilities_complete_t. */
    SDC_HCI_SUBEVENT_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE = 0x2c,
    /** @brief See @ref sdc_hci_subevent_le_cs_read_remote_fae_table_complete_t. */
    SDC_HCI_SUBEVENT_LE_CS_READ_REMOTE_FAE_TABLE_COMPLETE = 0x2d,
    /** @brief See @ref sdc_hci_subevent_le_cs_security_enable_complete_t. */
    SDC_HCI_SUBEVENT_LE_CS_SECURITY_ENABLE_COMPLETE = 0x2e,
    /** @brief See @ref sdc_hci_subevent_le_cs_config_complete_t. */
    SDC_HCI_SUBEVENT_LE_CS_CONFIG_COMPLETE = 0x2f,
    /** @brief See @ref sdc_hci_subevent_le_cs_procedure_enable_complete_t. */
    SDC_HCI_SUBEVENT_LE_CS_PROCEDURE_ENABLE_COMPLETE = 0x30,
    /** @brief See @ref sdc_hci_subevent_le_cs_subevent_result_t. */
    SDC_HCI_SUBEVENT_LE_CS_SUBEVENT_RESULT = 0x31,
    /** @brief See @ref sdc_hci_subevent_le_cs_subevent_result_continue_t. */
    SDC_HCI_SUBEVENT_LE_CS_SUBEVENT_RESULT_CONTINUE = 0x32,
    /** @brief See @ref sdc_hci_subevent_le_cs_test_end_complete_t. */
    SDC_HCI_SUBEVENT_LE_CS_TEST_END_COMPLETE = 0x33,
    /** @brief See @ref sdc_hci_subevent_le_frame_space_update_complete_t. */
    SDC_HCI_SUBEVENT_LE_FRAME_SPACE_UPDATE_COMPLETE = 0x35,
    /** @brief See @ref sdc_hci_subevent_le_conn_rate_change_t. */
    SDC_HCI_SUBEVENT_LE_CONN_RATE_CHANGE = 0x37,
};

/** @brief Advertising Event Properties parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t connectable_adv : 1;
    uint8_t scannable_adv : 1;
    uint8_t directed_adv : 1;
    uint8_t high_duty_cycle_adv : 1;
    uint8_t legacy_adv_packets : 1;
    uint8_t omit_adv_address : 1;
    uint8_t include_tx_power : 1;
    uint8_t use_decision_pdus : 1;
    uint8_t include_adva_in_ext_header : 1;
    uint8_t include_adi_in_ext_header : 1;
    uint8_t rfu : 6;
} sdc_hci_le_adv_event_properties_params_t;

/** @brief LE Advertising Report array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t event_type;
    uint8_t address_type;
    uint8_t address[6];
    uint8_t data_length;
    uint8_t data[];
    /* C does not allow fields to be placed after variable sized array
    int8_t rssi;
    */
} sdc_hci_le_adv_report_array_params_t;

/** @brief LE BIG Create Sync array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t bis;
} sdc_hci_le_big_create_sync_array_params_t;

/** @brief LE Create CIS array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t cis_conn_handle;
    uint16_t acl_conn_handle;
} sdc_hci_le_create_cis_array_params_t;

/** @brief LE CS Subevent Result array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t step_mode;
    uint8_t step_channel;
    uint8_t step_data_length;
    uint8_t step_data[];
} sdc_hci_le_cs_subevent_result_array_params_t;

/** @brief LE CS Subevent Result Continue array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t step_mode;
    uint8_t step_channel;
    uint8_t step_data_length;
    uint8_t step_data[];
} sdc_hci_le_cs_subevent_result_continue_array_params_t;

/** @brief LE Directed Advertising Report array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t event_type;
    uint8_t address_type;
    uint8_t address[6];
    uint8_t direct_address_type;
    uint8_t direct_address[6];
    int8_t rssi;
} sdc_hci_le_directed_adv_report_array_params_t;

/** @brief Event_Type parameters for HCI_LE_Extended_Advertising_Report event. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t connectable_adv : 1;
    uint16_t scannable_adv : 1;
    uint16_t directed_adv : 1;
    uint16_t scan_response : 1;
    uint16_t legacy_adv_pdus_used : 1;
    uint16_t data_status : 2;
    uint16_t rfu : 9;
} sdc_hci_le_ext_adv_report_array_event_type_params_t;

/** @brief LE Extended Advertising Report array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    union __PACKED __ALIGN(1) {
        sdc_hci_le_ext_adv_report_array_event_type_params_t params;
        uint8_t raw[2];
    } event_type;
    uint8_t address_type;
    uint8_t address[6];
    uint8_t primary_phy;
    uint8_t secondary_phy;
    uint8_t adv_sid;
    int8_t tx_power;
    int8_t rssi;
    uint16_t periodic_adv_interval;
    uint8_t direct_address_type;
    uint8_t direct_address[6];
    uint8_t data_length;
    uint8_t data[];
} sdc_hci_le_ext_adv_report_array_params_t;

/** @brief LE Extended Create Connection [v1] array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t scan_interval;
    uint16_t scan_window;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t max_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_length;
    uint16_t max_ce_length;
} sdc_hci_le_ext_create_conn_array_params_t;

/** @brief LE Extended Create Connection [v2] array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t scan_interval;
    uint16_t scan_window;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t max_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_length;
    uint16_t max_ce_length;
} sdc_hci_le_ext_create_conn_v2_array_params_t;

/** @brief LE event mask.
 *
 * If the bit in the Event_Mask is set to a one,
 * then the event associated with that bit will be enabled.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t le_connection_complete_event : 1;
    uint8_t le_advertising_report_event : 1;
    uint8_t le_connection_update_complete_event : 1;
    uint8_t le_read_remote_features_complete_event : 1;
    uint8_t le_long_term_key_request_event : 1;
    uint8_t le_remote_connection_parameter_request_event : 1;
    uint8_t le_data_length_change_event : 1;
    uint8_t le_read_local_p256_public_key_complete_event : 1;
    uint8_t le_generate_dhkey_complete_event : 1;
    uint8_t le_enhanced_connection_complete_event : 1;
    uint8_t le_directed_advertising_report_event : 1;
    uint8_t le_phy_update_complete_event : 1;
    uint8_t le_extended_advertising_report_event : 1;
    uint8_t le_periodic_advertising_sync_established_event : 1;
    uint8_t le_periodic_advertising_report_event : 1;
    uint8_t le_periodic_advertising_sync_lost_event : 1;
    uint8_t le_scan_timeout_event : 1;
    uint8_t le_advertising_set_terminated_event : 1;
    uint8_t le_scan_request_received_event : 1;
    uint8_t le_channel_selection_algorithm_event : 1;
    uint8_t le_connectionless_iq_report_event : 1;
    uint8_t le_connection_iq_report_event : 1;
    uint8_t le_cte_request_failed_event : 1;
    uint8_t le_periodic_advertising_sync_transfer_received_event : 1;
    uint8_t le_cis_established_event : 1;
    uint8_t le_cis_request_event : 1;
    uint8_t le_create_big_complete_event : 1;
    uint8_t le_terminate_big_complete_event : 1;
    uint8_t le_big_sync_established_event : 1;
    uint8_t le_big_sync_lost_event : 1;
    uint8_t le_request_peer_sca_complete_event : 1;
    uint8_t le_path_loss_threshold_event : 1;
    uint8_t le_transmit_power_reporting_event : 1;
    uint8_t le_biginfo_advertising_report_event : 1;
    uint8_t le_subrate_change_event : 1;
    uint8_t le_periodic_advertising_sync_established_event_v2 : 1;
    uint8_t le_periodic_advertising_report_event_v2 : 1;
    uint8_t le_periodic_advertising_sync_transfer_received_event_v2 : 1;
    uint8_t le_periodic_advertising_subevent_data_request_event : 1;
    uint8_t le_periodic_advertising_response_report_event : 1;
    uint8_t le_enhanced_connection_complete_event_v2 : 1;
} sdc_hci_le_le_event_mask_t;

/** @brief LE features.
 *
 * See Core Specification, Vol 6, Part B, Section 4.6
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t le_encryption : 1;
    uint8_t connection_parameters_request : 1;
    uint8_t extended_reject_indication : 1;
    uint8_t slave_initiated_features_exchange : 1;
    uint8_t le_ping : 1;
    uint8_t le_data_packet_length_extension : 1;
    uint8_t ll_privacy : 1;
    uint8_t extended_scanner_filter_policies : 1;
    uint8_t le_2m_phy : 1;
    uint8_t stable_modulation_index_transmitter : 1;
    uint8_t stable_modulation_index_receiver : 1;
    uint8_t le_coded_phy : 1;
    uint8_t le_extended_advertising : 1;
    uint8_t le_periodic_advertising : 1;
    uint8_t channel_selection_algorithm_2 : 1;
    uint8_t le_Power_class_1 : 1;
    uint8_t minimum_number_of_used_channels_procedure : 1;
    uint8_t connection_cte_request : 1;
    uint8_t connection_cte_response : 1;
    uint8_t connectionless_cte_transmitter : 1;
    uint8_t connectionless_cte_receiver : 1;
    uint8_t antenna_switching_during_cte_transmission_aod : 1;
    uint8_t antenna_switching_during_cte_reception_aoa : 1;
    uint8_t receiving_constant_tone_extensions : 1;
    uint8_t periodic_advertising_sync_transfer_sender : 1;
    uint8_t periodic_advertising_sync_transfer_recipient : 1;
    uint8_t sleep_clock_accuracy_updates : 1;
    uint8_t remote_public_key_validation : 1;
    uint8_t connected_isochronous_stream_master : 1;
    uint8_t connected_isochronous_stream_slave : 1;
    uint8_t isochronous_broadcaster : 1;
    uint8_t synchronized_receiver : 1;
    uint8_t isochronous_channels_host_support : 1;
    uint8_t le_power_control_request : 1;
    uint8_t le_power_change_indication : 1;
    uint8_t le_path_loss_monitoring : 1;
    uint8_t periodic_advertising_adi_support : 1;
    uint8_t connection_subrating : 1;
    uint8_t connection_subrating_host_support : 1;
    uint8_t channel_classification : 1;
    uint8_t advertising_coding_selection : 1;
    uint8_t advertising_coding_selection_host_support : 1;
    uint8_t decision_based_advertising_filtering : 1;
    uint8_t periodic_advertising_with_responses_advertiser : 1;
    uint8_t periodic_advertising_with_responses_scanner : 1;
    uint8_t unsegmented_framed_mode : 1;
    uint8_t channel_sounding : 1;
    uint8_t channel_sounding_host_support : 1;
    uint8_t channel_sounding_tone_quality_indication : 1;
    uint8_t rfu56 : 1;
    uint8_t rfu57 : 1;
    uint8_t rfu58 : 1;
    uint8_t rfu59 : 1;
    uint8_t rfu60 : 1;
    uint8_t rfu61 : 1;
    uint8_t rfu62 : 1;
    uint8_t ll_extended_feature_set : 1;
} sdc_hci_le_le_features_t;

/** @brief Options parameters for HCI_LE_Periodic_Advertising_Create_Sync command. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t use_periodic_adv_list : 1;
    uint8_t disable_reporting : 1;
    uint8_t duplicate_filter_initially_enabled : 1;
    uint8_t rfu : 5;
} sdc_hci_le_periodic_adv_create_sync_options_params_t;

/** @brief LE Read Minimum Supported Connection Interval groups. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t group_min;
    uint16_t group_max;
    uint16_t group_stride;
} sdc_hci_le_read_min_supported_conn_interval_group_t;

/** @brief LE Set CIG Parameters array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cis_id;
    uint16_t max_sdu_c_to_p;
    uint16_t max_sdu_p_to_c;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t rtn_c_to_p;
    uint8_t rtn_p_to_c;
} sdc_hci_le_set_cig_params_array_params_t;

/** @brief LE Set CIG Parameters output array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_le_set_cig_params_output_array_params_t;

/** @brief LE Set CIG Parameters Test array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cis_id;
    uint8_t nse;
    uint16_t max_sdu_c_to_p;
    uint16_t max_sdu_p_to_c;
    uint16_t max_pdu_c_to_p;
    uint16_t max_pdu_p_to_c;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t bn_c_to_p;
    uint8_t bn_p_to_c;
} sdc_hci_le_set_cig_params_test_array_params_t;

/** @brief LE Set CIG Parameters Test output array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_le_set_cig_params_test_output_array_params_t;

/** @brief LE Set Connection CTE Transmit Parameters array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t antenna_ids;
} sdc_hci_le_set_conn_cte_transmit_params_array_params_t;

/** @brief LE Set Connectionless CTE Transmit Parameters array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t antenna_ids;
} sdc_hci_le_set_connless_cte_transmit_params_array_params_t;

/** @brief Change Reasons parameters for HCI_LE_Set_Data_Related_Address_Changes command. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t change_on_adv_data_change : 1;
    uint8_t change_on_scan_response_data_change : 1;
    uint8_t rfu : 6;
} sdc_hci_le_set_data_related_address_changes_reasons_params_t;

/** @brief LE Set Extended Advertising Enable array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint16_t duration;
    uint8_t max_ext_adv_events;
} sdc_hci_le_set_ext_adv_enable_array_params_t;

/** @brief LE Set Extended Scan Parameters array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t scan_type;
    uint16_t scan_interval;
    uint16_t scan_window;
} sdc_hci_le_set_ext_scan_params_array_params_t;

/** @brief Enable parameters for HCI_LE_Set_Periodic_Advertising_Enable command. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t enable : 1;
    uint8_t include_adi : 1;
    uint8_t rfu : 6;
} sdc_hci_le_set_periodic_adv_enable_params_t;

/** @brief Enable parameters for HCI_LE_Set_Periodic_Advertising_Receive_Enable command. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t enable_reporting : 1;
    uint8_t enable_duplicate_filtering : 1;
    uint8_t rfu : 6;
} sdc_hci_le_set_periodic_adv_receive_enable_params_t;

/** @brief LE Set Periodic Advertising Subevent Data array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t subevent;
    uint8_t response_slot_start;
    uint8_t response_slot_count;
    uint8_t subevent_data_length;
    uint8_t subevent_data[];
} sdc_hci_le_set_periodic_adv_subevent_data_array_params_t;

/** @brief Remaining parameters for the transmitter test v4. */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t tx_power_level;
} sdc_hci_le_transmitter_test_v4_tx_power_level_t;

/** @} end of HCI_TYPES */

/**
 * @defgroup HCI_EVENTS Events
 * @{
 */

/** @brief LE Advertising Report.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.2
 *
 * This event indicates that one or more Bluetooth devices have responded to an active
 * scan or have broadcast advertisements that were received during a passive scan. The
 * Controller may queue these advertising reports and send information from multiple
 * devices in one HCI_LE_Advertising_Report event.
 *
 * This event shall only be generated if scanning was enabled using the
 * HCI_LE_Set_Scan_Enable command. It only reports advertising events that used
 * legacy advertising PDUs.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t num_reports;
    /** C does not allow array of variable sized arrays, otherwise it should be @ref
    sdc_hci_le_adv_report_array_params_t reports[]; */
    uint8_t reports[];
} sdc_hci_subevent_le_adv_report_t;

/** @brief LE Directed Advertising Report.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.11
 *
 * This event indicates that directed advertisements have been received where the
 * advertiser is using a resolvable private address for the TargetA field of the advertising
 * PDU which the Controller is unable to resolve and the Scanning_Filter_Policy is equal
 * to 0x02 or 0x03, see Section 7.8.10. Direct_Address_Type and Direct_Address specify
 * the address the directed advertisements are being directed to. Address_Type and
 * Address specify the address of the advertiser sending the directed advertisements.
 * The Controller may queue these advertising reports and send information from multiple
 * advertisers in one HCI_LE_Directed_Advertising_Report event.
 *
 * This event shall only be generated if scanning was enabled using the
 * HCI_LE_Set_Scan_Enable command. It only reports advertising events that used
 * legacy advertising PDUs.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t num_reports;
    sdc_hci_le_directed_adv_report_array_params_t reports[];
} sdc_hci_subevent_le_directed_adv_report_t;

/** @brief LE Extended Advertising Report.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.13
 *
 * This event indicates that one or more Bluetooth devices have responded to an active
 * scan or have broadcast advertisements that were received during a passive scan.
 * The Controller may coalesce multiple advertising reports from the same or different
 * advertisers into a single HCI_LE_Extended_Advertising_Report event, provided all the
 * parameters from all the advertising reports fit in a single HCI event.
 *
 * This event shall only be generated if scanning was enabled using the HCI_LE_Set_-
 * Extended_Scan_Enable command. It reports advertising events using either legacy or
 * extended advertising PDUs.
 *
 * The Controller may split the data from a single advertisement or scan response
 * (whether one PDU or several) into several reports. If so, each report except the
 * last shall have an Event_Type with a data status field of "incomplete, more data to
 * come", while the last shall have the value "complete"; the Address_Type, Address,
 * Advertising_SID, Primary_PHY, and Secondary_PHY fields shall be the same in all the
 * reports. No further reports shall be sent for a given advertisement or scan response
 * after one with a Data_Status other than "incomplete, more data to come".
 *
 * When a scan response is received, bits 0 to 2 and 4 of the event type shall indicate the
 * properties of the original advertising event and the Advertising_SID field should be set
 * to the value in the original scannable advertisement.
 * An Event_Type with a data status field of "incomplete, data truncated" shall indicate that
 * the Controller attempted to receive an AUX_CHAIN_IND PDU but was not successful or
 * received it but was unable to store the data.
 *
 * Where the event being reported used a legacy advertising PDU, the Controller shall set
 * the Event_Type to the value specified in Table 7.1.
 *
 *  PDU Type                                                        Event_Type
 *  ADV_IND                                                         0b0010011
 *  ADV_DIRECT_IND                                                  0b0010101
 *  ADV_SCAN_IND                                                    0b0010010
 *  ADV_NONCONN_IND                                                 0b0010000
 *  SCAN_RSP to an ADV_IND                                          0b0011011
 *  SCAN_RSP to an ADV_SCAN_IND                                     0b0011010
 * Table 7.1: Event_Type values for legacy PDUs
 *
 * If the Event_Type indicates a legacy PDU (bit 4 = 1), the Primary_PHY parameter
 * shall indicate the LE 1M PHY and the Secondary_PHY parameter shall be set to
 * 0x00. Otherwise, the Primary_PHY parameter shall indicate the PHY used to send the
 * advertising PDU on the primary advertising physical channel and the Secondary_PHY
 * parameter shall indicate the PHY used to send the advertising PDU(s), if any, on
 * the secondary advertising physical channel. If the Advertising Coding Selection (Host
 * Support) Link Layer feature bit is set (see [Vol 6] Part B, Section 4.6) and the
 * Primary_PHY or Secondary_PHY parameter indicates that the LE Coded PHY was
 * used, then the parameter shall also indicate which coding was used.
 *
 * The Periodic_Advertising_Interval parameter shall be set to zero when no periodic
 * advertising exists as part of the advertising set.
 *
 * The Direct_Address_Type and Direct_Address parameters shall contain the TargetA
 * address in the advertising PDU for directed advertising event types (bit 2 = 1). These
 * parameters shall be ignored for undirected advertising event types (bit 2 = 0). If
 * the TargetA address is a resolvable private address that the Controller successfully
 * resolved, then the value of Direct_Address_Type shall depend on the value of the
 * Own_Address_Type parameter of the command that set the extended scan parameters.
 * Direct_Address shall be set as follows:
 *
 * • If Direct_Address_Type equals 0x02, then Direct_Address shall be set to either the
 *   TargetA field in the received advertisement or to the public device address of the
 *   scanning device.
 * • If Direct_Address_Type equals 0x03, then Direct_Address shall be set to either
 *   the TargetA field in the received advertisement or to the address set by the
 *   HCI_LE_Set_Random_Address command.
 * • Otherwise Direct_Address shall be set to the TargetA field in the received
 *   advertisement.
 *
 * When multiple advertising packets are used to complete a single advertising report
 * (e.g., a packet containing an ADV_EXT_IND PDU combined with one containing an
 * AUX_ADV_IND PDU), the RSSI parameter shall be set based on the last packet
 * received and the TX_Power parameter shall be based on the last packet of the current
 * advertisement or scan response received that contains a TxPower field. If there is no
 * packet containing a TxPower field, then TX_Power shall be set to 0x7F. However, if
 * an event has been sent with a TX_Power value other than 0x7F and a Data_Status of
 * "incomplete, more data to come", and if no subsequent PDU with a TxPower field has
 * been received, then subsequent events may instead have a TX_Power value of 0x7F.
 *
 * If the Controller receives an AUX_CHAIN_IND with no AdvData, it should send the
 * report (or the last report if it has split the data) immediately without waiting for any
 * subsequent AUX_CHAIN_IND PDUs.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t num_reports;
    /** C does not allow array of variable sized arrays, otherwise it should be @ref
    sdc_hci_le_ext_adv_report_array_params_t reports[]; */
    uint8_t reports[];
} sdc_hci_subevent_le_ext_adv_report_t;

/** @brief LE CIS Established.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.25
 *
 * This event indicates that a CIS has been established, was considered lost before being
 * established, or—on the Central—was rejected by the Peripheral. It is generated by the
 * Controller in the Central and Peripheral. The Connection_Handle parameter shall be set
 * to the value provided in the HCI_LE_Create_CIS command on the Central and in the
 * HCI_LE_CIS_Request event on the Peripheral.
 *
 * The CIG_Sync_Delay parameter is the maximum time, in microseconds, for
 * transmission of PDUs of all CISes in a CIG event (see [Vol 6] Part B, Section 4.5.14.1).
 *
 * The CIS_Sync_Delay parameter is the maximum time, in microseconds, for
 * transmission of PDUs of the specified CIS in a CIG event (see [Vol 6] Part B,
 * Section 4.5.14.1).
 *
 * The Transport_Latency_C_To_P and Transport_Latency_P_To_C parameters are the
 * actual transport latencies, in microseconds, as described in [Vol 6] Part G, Section 3.2.1
 * and [Vol 6] Part G, Section 3.2.2.
 *
 * The PHY_C_To_P parameter indicates the PHY selected for packets from the Central to
 * Peripheral.
 * The PHY_P_To_C parameter indicates the PHY selected for packets from the
 * Peripheral to Central.
 *
 * The remaining parameters are the corresponding parameters of the CIS (see [Vol 6]
 * Part B, Section 4.5.13.1).
 *
 * If this event is generated on the Peripheral with a non-zero status, the Controller shall
 * delete the Connection_Handle and any associated ISO data paths.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint32_t cig_sync_delay : 24;
    uint32_t cis_sync_delay : 24;
    uint32_t transport_latency_c_to_p : 24;
    uint32_t transport_latency_p_to_c : 24;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t nse;
    uint8_t bn_c_to_p;
    uint8_t bn_p_to_c;
    uint8_t ft_c_to_p;
    uint8_t ft_p_to_c;
    uint16_t max_pdu_c_to_p;
    uint16_t max_pdu_p_to_c;
    uint16_t iso_interval;
} sdc_hci_subevent_le_cis_established_t;

/** @brief LE Subrate Change.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.35
 *
 * This event is used to indicate that a Connection Subrate Update procedure has
 * completed and some parameters of the specified connection have changed.
 *
 * This event shall be issued if the HCI_LE_Subrate_Request command was issued by
 * the Host or the parameters are updated successfully following a request from the peer
 * device. If no parameters are updated following a request from the peer device or the
 * parameters were changed using the Connection Update procedure or the Connection
 * Rate Update procedure, then this event shall not be issued.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint16_t subrate_factor;
    uint16_t peripheral_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
} sdc_hci_subevent_le_subrate_change_t;

/** @brief LE CIS Established [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.25
 *
 * This event indicates that a CIS has been established, was considered lost before being
 * established, or—on the Central—was rejected by the Peripheral. It is generated by the
 * Controller in the Central and Peripheral. The Connection_Handle parameter shall be set
 * to the value provided in the HCI_LE_Create_CIS command on the Central and in the
 * HCI_LE_CIS_Request event on the Peripheral.
 *
 * The CIG_Sync_Delay parameter is the maximum time, in microseconds, for
 * transmission of PDUs of all CISes in a CIG event (see [Vol 6] Part B, Section 4.5.14.1).
 *
 * The CIS_Sync_Delay parameter is the maximum time, in microseconds, for
 * transmission of PDUs of the specified CIS in a CIG event (see [Vol 6] Part B,
 * Section 4.5.14.1).
 *
 * The Transport_Latency_C_To_P and Transport_Latency_P_To_C parameters are the
 * actual transport latencies, in microseconds, as described in [Vol 6] Part G, Section 3.2.1
 * and [Vol 6] Part G, Section 3.2.2.
 *
 * The PHY_C_To_P parameter indicates the PHY selected for packets from the Central to
 * Peripheral.
 * The PHY_P_To_C parameter indicates the PHY selected for packets from the
 * Peripheral to Central.
 *
 * The remaining parameters are the corresponding parameters of the CIS (see [Vol 6]
 * Part B, Section 4.5.13.1).
 *
 * If this event is generated on the Peripheral with a non-zero status, the Controller shall
 * delete the Connection_Handle and any associated ISO data paths.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint32_t cig_sync_delay : 24;
    uint32_t cis_sync_delay : 24;
    uint32_t transport_latency_c_to_p : 24;
    uint32_t transport_latency_p_to_c : 24;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t nse;
    uint8_t bn_c_to_p;
    uint8_t bn_p_to_c;
    uint8_t ft_c_to_p;
    uint8_t ft_p_to_c;
    uint16_t max_pdu_c_to_p;
    uint16_t max_pdu_p_to_c;
    uint16_t iso_interval;
    uint32_t sub_interval : 24;
    uint16_t max_sdu_c_to_p;
    uint16_t max_sdu_p_to_c;
    uint32_t sdu_interval_c_to_p : 24;
    uint32_t sdu_interval_p_to_c : 24;
    uint8_t framing;
} sdc_hci_subevent_le_cis_established_v2_t;

/** @brief LE Read All Remote Features Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.38
 *
 * This event is used to indicate the completion of the process of the Controller
 * obtaining the features supported by the remote Bluetooth device specified by the
 * Connection_Handle event parameter.
 *
 * The Max_Remote_Page parameter specifies the highest-numbered page of the remote
 * device’s supported LE features that contains at least one bit set to 1; all higher-number
 * pages therefore only contain zeroes. The Max_Valid_Page parameter specifies the
 * highest-numbered page of features that the Controller has obtained from the remote
 * device or, if it has obtained all pages from 1 to Max_Remote_Page, then any value
 * greater than or equal to Max_Remote_Page.
 *
 * The LE_Features parameter contains the LE features. The Controller shall set all pages
 * between 0 and Max_Valid_Page to valid data and shall set all higher-numbered pages
 * to all zero bits.
 *
 * Note: If Max_Valid_Page ≥ Max_Remote_Page, then all pages will contain valid data,
 * which will be all zero bits for pages numbered greater than Max_Remote_Page.
 *
 * If the feature mask is requested more than once while a connection exists between the
 * two devices, then the second and subsequent requests may report a cached copy of the
 * feature mask rather than fetching the feature mask again.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t max_remote_page;
    uint8_t max_valid_page;
    uint8_t le_features[248];
} sdc_hci_subevent_le_read_all_remote_features_complete_t;

/** @brief LE CS Read Remote Supported Capabilities Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.39
 *
 * This event shall be generated when a locally initiated CS Capabilities Exchange
 * procedure has completed or when the local Controller has received an
 * LL_CS_CAPABILITIES_REQ from the remote Controller.
 *
 * The Num_Config_Supported parameter indicates the number of CS configurations that
 * are supported by the remote Controller.
 *
 * The Max_Consecutive_Procedures_Supported parameter indicates the maximum
 * number of consecutive CS procedures that are supported by the remote Controller.
 *
 * The Num_Antennae_Supported parameter indicates the number of antenna elements
 * that are available for CS tone exchanges.
 * The Max_Antenna_Paths_Supported parameter indicates the maximum number of
 * antenna paths that are supported by the remote Controller for CS tone exchanges.
 *
 * The Roles_Supported parameter indicates the CS roles that are supported by the
 * remote Controller.
 *
 * The Modes_Supported parameter indicates the optional CS modes that are supported
 * by the remote Controller.
 *
 * The RTT_Capability, RTT_AA_Only_N, RTT_Sounding_N, and RTT_Random_-
 * Payload_N parameters indicate the time-of-flight accuracy as described in [Vol 6] Part
 * B, Section 2.4.2.44.
 *
 * The NADM_Sounding_Capability and NADM_Random_Capability parameters indicate
 * the support by the remote Controller for reporting Normalized Attack Detector Metric
 * (NADM) when a CS_SYNC with a sounding sequence or random sequence is received.
 *
 * The CS_SYNC_PHYs_Supported parameter indicates the optional transmit and receive
 * PHYs supported by the remote Controller for CS_SYNC exchanges as described in [Vol
 * 6] Part H, Section 4.3.
 *
 * The Subfeatures_Supported parameter indicates which of the following optional
 * subfeatures are supported by the remote Controller:
 *
 * • A Frequency Actuation Error of zero for all allowed CS channels as described in [Vol
 *   6] Part A, Section 3.5.
 * • Channel Selection Algorithm #3c as described in [Vol 6] Part H, Section 4.1.4.2.
 * • Phase-based ranging from a sounding sequence as described in [Vol 6] Part H,
 *   Section 3.3.1.
 *
 * The T_IP1_Times_Supported, T_IP2_Times_Supported, T_FCS_Times_Supported,
 * T_PM_Times_Supported, and T_SW_Time_Supported parameters indicate the
 * supported optional time durations used in CS steps as described in [Vol 6] Part H,
 * Section 4.3.
 *
 * The TX_SNR_Capability parameter indicated the supported SNR levels used for the
 * CS_SYNC packets used in mode-1 and mode-3 steps as described in [Vol 6] Part A,
 * Section 3.1.3.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t num_config_supported;
    uint16_t max_consecutive_procedures_supported;
    uint8_t num_antennas_supported;
    uint8_t max_antenna_paths_supported;
    uint8_t roles_supported;
    uint8_t modes_supported;
    uint8_t rtt_capability;
    uint8_t rtt_aa_only_n;
    uint8_t rtt_sounding_n;
    uint8_t rtt_random_payload_n;
    uint16_t nadm_sounding_capability;
    uint16_t nadm_random_capability;
    uint8_t cs_sync_phys_supported;
    uint16_t subfeatures_supported;
    uint16_t t_ip1_times_supported;
    uint16_t t_ip2_times_supported;
    uint16_t t_fcs_times_supported;
    uint16_t t_pm_times_supported;
    uint8_t t_sw_time_supported;
    uint8_t tx_snr_capability;
} sdc_hci_subevent_le_cs_read_remote_supported_capabilities_complete_t;

/** @brief LE CS Read Remote FAE Table Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.40
 *
 * This event shall be generated when a locally initiated CS Mode-0 Frequency Actuation
 * Table Update procedure has completed.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t remote_fae_table[72];
} sdc_hci_subevent_le_cs_read_remote_fae_table_complete_t;

/** @brief LE CS Security Enable Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.41
 *
 * This event shall be generated when a locally initiated CS Security Start procedure has
 * completed or when the local Controller has responded to a CS security request from the
 * remote Controller.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
} sdc_hci_subevent_le_cs_security_enable_complete_t;

/** @brief LE CS Config Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.42
 *
 * This event shall be generated when a locally initiated Channel Sounding Configuration
 * procedure has completed or when the local Controller has responded to a CS
 * configuration request from the remote Controller for the CS configuration identified by
 * Config_ID or when a CS configuration is created only with local context. The Action
 * parameter indicates if a CS configuration was requested to be created or removed. The
 * Status parameter indicates whether the request indicated by the Action parameter was
 * successful.
 *
 * When the Action parameter is set to 0x00, all the remaining event parameters are
 * ignored.
 * The Main_Mode_Type and Sub_Mode_Type parameters indicate the CS modes used
 * during the CS procedure for the specified CS configuration. The Min_Main_Mode_Steps
 * and Max_Main_Mode_Steps parameters indicate the range of main mode CS steps
 * executed before a submode CS step is executed during the CS procedure. The
 * Main_Mode_Repetition parameter indicates the number of main mode CS steps
 * repeated from the last CS subevent at the beginning of the current CS subevent. The
 * Mode_0_Steps parameter indicates the number of mode-0 CS steps included at the
 * beginning of each CS subevent.
 *
 * The Role parameter indicates the CS role for the local Controller for the specified CS
 * configuration. The RTT_Type parameter indicates the RTT variant to be used during the
 * CS procedure, and the CS_SYNC_PHY parameter indicates the PHY to be used for
 * CS_SYNC exchanges during the CS procedure for the specified CS configuration.
 *
 * The Channel_Map parameter indicates the channels to be used or unused during the
 * CS procedure, and the Channel_Map_Repetition parameter indicates the number of
 * times the channels specified by Channel_Map will be repeated for non-mode-0 steps
 * during the CS procedure (see [Vol 6] Part H, Section 4.1.4).
 *
 * The Channel_Selection_Type parameter indicates the Channel Selection Algorithm
 * to be used during the CS procedure for non-mode-0 steps. When the
 * Channel_Selection_Type parameter is set to 0x01, the Ch3c_Shape and the
 * Ch3c_Jump parameters indicate the selected shape and channels to be skipped as
 * described in [Vol 6] Part H, Section 4.1.4.2.
 *
 * The T_IP1_Time, T_IP2_Time, T_FCS_Time, T_PM_Time, and T_SW_Time
 * parameters indicate the time durations used in CS steps as described in [Vol 6] Part
 * H, Section 4.3.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t config_id;
    uint8_t action;
    uint8_t main_mode_type;
    uint8_t sub_mode_type;
    uint8_t min_main_mode_steps;
    uint8_t max_main_mode_steps;
    uint8_t main_mode_repetition;
    uint8_t mode_0_steps;
    uint8_t role;
    uint8_t rtt_type;
    uint8_t cs_sync_phy;
    uint8_t channel_map[10];
    uint8_t channel_map_repetition;
    uint8_t channel_selection_type;
    uint8_t ch3c_shape;
    uint8_t ch3c_jump;
    uint8_t reserved;
    uint8_t t_ip1_time;
    uint8_t t_ip2_time;
    uint8_t t_fcs_time;
    uint8_t t_pm_time;
} sdc_hci_subevent_le_cs_config_complete_t;

/** @brief LE CS Procedure Enable Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.43
 *
 * This event shall be generated when the local or remote Controller has scheduled a
 * new CS procedure or disabled an ongoing CS procedure as a result of an HCI_LE_-
 * CS_Procedure_Enable command. When a new CS procedure is enabled, the HCI_LE_-
 * CS_Procedure_Enable_Complete event shall be sent to the Host after the LL_CS_IND
 * is transmitted or received and before any CS subevent results are available.
 *
 * When the State parameter is set to 0x00, all the remaining event parameters are
 * ignored.
 *
 * The Tone_Antenna_Config_Selection parameter indicates the Antenna Configuration
 * Index used in the CS procedure.
 *
 * The Controller shall set the Selected_TX_Power parameter to the transmit power level
 * that it will use for the CS procedures as described in [Vol 6] Part B, Section 5.1.26. If the
 * radiated power level will vary between packets (e.g., because of frequency-dependent
 * properties of the transmitter), then the value should be the best estimate of the transmit
 * power level that will be used.
 *
 * The Subevent_Len parameter indicates the selected maximum duration of each CS
 * subevent during the CS procedure. The Subevents_Per_Event parameter indicates
 * the number of CS subevents that are anchored off the same associated LE ACL
 * anchor point. The Subevent_Interval parameter indicates the gap between the start
 * of two consecutive CS subevents that are anchored off the same associated LE
 * ACL anchor point. The Event_Interval parameter indicates the number of connection
 * intervals between consecutive LE ACL anchor points from which CS subevents are
 * anchored.
 *
 * The Procedure_Interval parameter indicates the selected interval between consecutive
 * CS procedures, in units of ACL connection events. The Procedure_Count parameter
 * indicates the selected number of consecutive CS procedures to be scheduled as
 * part of this measurement. The Max_Procedure_Len parameter indicates the selected
 * maximum duration of each CS procedure.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t config_id;
    uint8_t state;
    uint8_t tone_antenna_config_selection;
    int8_t selected_tx_power;
    uint32_t subevent_len : 24;
    uint8_t subevents_per_event;
    uint16_t subevent_interval;
    uint16_t event_interval;
    uint16_t procedure_interval;
    uint16_t procedure_count;
    uint16_t max_procedure_len;
} sdc_hci_subevent_le_cs_procedure_enable_complete_t;

/** @brief LE CS Subevent Result.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.44
 *
 * This event shall be generated when the local Controller has results to report for a CS
 * subevent during the CS procedure. Depending on the number of CS steps in the CS
 * subevent, the Controller may choose to report complete or partial results. When the
 * number of CS steps exceeds the maximum HCI event size, the Controller may report
 * further results for the CS subevent using the HCI_LE_CS_Subevent_Result_Continue
 * event.
 *
 * When Connection_Handle is set to 0x0FFF, the Config_ID and Start_ACL_Conn_-
 * Event_Counter parameters shall be ignored.
 *
 * The Start_ACL_Conn_Event_Counter parameter indicates the starting ACL connection
 * event count from which the CS event results reported in this HCI event are anchored. A
 * CS procedure may have CS subevents in multiple ACL connection events anchored
 * to the same ACL connection event count as defined by the Event_Interval return
 * parameter in the HCI_LE_CS_Procedure_Enable_Complete event.
 *
 * The Procedure_Counter parameter indicates the associated CS procedure count for the
 * results reported in this HCI event (see [Vol 6] Part B, Section 5.1.24).
 * The Frequency_Compensation parameter indicates the value of the fractional frequency
 * offset (FFO) used by the initiator device to align the timing of CS steps and transmit
 * frequencies during non-mode-0 CS steps (see [Vol 6] Part A, Section 3.5.1).
 *
 * The Reference_Power_Level parameter is expressed in dBm and is described in [Vol 6]
 * Part H, Section 4.6 and [Vol 6] Part H, Section 3.3.1. If the reference power level value
 * is not available during a subevent, then this value shall be set to 0x7F.
 *
 * When bits 0 to 3 of Subevent_Done_Status are set to 0x1, the Controller shall send
 * one or more LE CS Subevent Result Continue events for the current CS subevent.
 * Otherwise, the Controller sends no further events for the current CS subevent.
 *
 * When bits 0 to 3 of Procedure_Done_Status are set to 0x1, the Controller shall send
 * one or more LE CS Subevent Result or LE CS Subevent Result Continue events for the
 * current or subsequent CS procedure. Otherwise, the Controller sends no further events
 * for the current CS procedure. When bits 0 to 3 of Procedure_Done_Status are set to
 * 0xF or when results of all CS procedures are sent to the Host, the Controller sends no
 * further events until a new CS procedure is enabled by the Host.
 *
 * The allowed combinations of Procedure_Done_Status and Subevent_Done_Status are
 * shown in Table 7.2.
 *
 *  Procedure_Done_Status                  Allowed values of Subevent_Done_Status
 *  (Bits 0 to 3)                          (Bits 0 to 3)
 *  0x0                                    0x0, 0xF
 *  0x1                                    0x0, 0x1, 0xF
 *  0xF                                    0x0, 0xF
 * Table 7.2: Allowed combinations of done status
 *
 * The Num_Antenna_Paths parameter indicates the number of antenna paths used for
 * CS tone exchanges.
 *
 * The Num_Steps_Reported parameter indicates the number of CS steps for which
 * results are reported. The Step_Mode[i] parameter indicates the CS mode for each CS
 * step, and the Step_Channel[i] parameter indicates the channel used for each CS step.
 * A Controller may return a value 0 when a subevent is aborted.
 *
 * The Step_Data_Length[i] and Step_Data[i] parameters indicate the reported data
 * that varies based on the CS mode and the CS role of the local device. When
 * Step_Data_Length[i] is set to zero, it indicates that the Step may have been aborted for
 * unspecified reasons. Otherwise, the Step_Data[i] indicates the mode- and role-specific
 * information being reported.
 *
 * For each CS procedure in a repeat sequence, the Controller shall report the results
 * for each CS step in each CS subevent of the CS procedure even if the CS step has
 * been aborted for any reason. In order to terminate reporting of pending CS steps in
 * a CS subevent, the Controller shall set the Subevent_Done_Status parameter to 0xF
 * in the HCI_LE_CS_Subevent_Result or the HCI_LE_CS_Subevent_Result_Continue
 * event and shall not report any additional HCI_LE_CS_Subevent_Result_Continue
 * events for that CS subevent. The Controller may terminate reporting of pending
 * CS procedures by setting the Procedure_Done_Status parameter to 0xF in the
 * HCI_LE_CS_Subevent_Result or the HCI_LE_CS_Subevent_Result_Continue event
 * and shall not report any additional HCI_LE_CS_Subevent_Result_Continue events for
 * that CS procedure.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t config_id;
    uint16_t start_acl_conn_event_counter;
    uint16_t procedure_counter;
    uint16_t frequency_compensation;
    int8_t reference_power_level;
    uint8_t procedure_done_status;
    uint8_t subevent_done_status;
    uint8_t abort_reason;
    uint8_t num_antenna_paths;
    uint8_t num_steps_reported;
    /** C does not allow array of variable sized arrays, otherwise it should be @ref
    sdc_hci_le_cs_subevent_result_array_params_t data[]; */
    uint8_t data[];
} sdc_hci_subevent_le_cs_subevent_result_t;

/** @brief LE CS Subevent Result Continue.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.45
 *
 * This event shall be generated after the local Controller has completed a new CS
 * subevent measurement and has already sent an HCI_LE_CS_Subevent_Result event
 * for the specified CS subevent.
 *
 * When Connection_Handle is set to 0x0FFF, the Config_ID parameter shall be ignored.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t config_id;
    uint8_t procedure_done_status;
    uint8_t subevent_done_status;
    uint8_t abort_reason;
    uint8_t num_antenna_paths;
    uint8_t num_steps_reported;
    /** C does not allow array of variable sized arrays, otherwise it should be @ref
    sdc_hci_le_cs_subevent_result_continue_array_params_t data[]; */
    uint8_t data[];
} sdc_hci_subevent_le_cs_subevent_result_continue_t;

/** @brief LE CS Test End Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.46
 *
 * This event shall be generated when the local Controller has stopped an ongoing CS test
 * as a result of the HCI_LE_CS_Test_End command. When the HCI_LE_CS_Test_End
 * command is issued by the Host, the HCI_LE_CS_Test_End_Complete event shall be
 * sent to the Host after any pending CS test result events are sent to the Host.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
} sdc_hci_subevent_le_cs_test_end_complete_t;

/** @brief LE Frame Space Update Complete.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.48
 *
 * This event is used to indicate that the Frame Space Update procedure has completed
 * (see [Vol 6] Part B, Section 5.1.30) and, if initiated autonomously by the local Controller
 * or the peer device, that at least one frame space value has changed.
 *
 * The Initiator parameter indicates who initiated the Frame Space Update procedure.
 *
 * The Frame_Space parameter indicates the new frame space value that the Controller is
 * now using.
 *
 * The PHYS and Spacing_Types parameters indicate which PHYs and spacing types are
 * using the new frame space value.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t initiator;
    uint16_t frame_space;
    uint8_t phys;
    uint16_t spacing_types;
} sdc_hci_subevent_le_frame_space_update_complete_t;

/** @brief LE Connection Rate Change.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.65.50
 *
 * The HCI_LE_Connection_Rate_Change event is used to indicate that the Connection
 * Rate Update procedure has completed.
 *
 * The event shall be issued if the HCI_LE_Connection_Rate_Request command was
 * issued by the Host or if the connection parameters are updated following a request
 * from the peer device. If no parameters are updated following a request from the peer
 * device or the parameters were changed using the Connection Update procedure or the
 * Connection Subrate Update procedure, then this event shall not be issued.
 *
 * Note: The parameter values returned in this event can be different from the parameter
 * values provided by the Host through the HCI_LE_Connection_Rate_Request command
 * (Section 7.8.154).
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t status;
    uint16_t conn_handle;
    uint16_t conn_interval;
    uint16_t subrate_factor;
    uint16_t peripheral_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
} sdc_hci_subevent_le_conn_rate_change_t;

/** @} end of HCI_EVENTS */

/**
 * @defgroup HCI_COMMAND_PARAMETERS Parameters
 * @{
 */

/** @brief LE Set Event Mask command parameter(s). */
typedef union __PACKED __ALIGN(1)
{
    sdc_hci_le_le_event_mask_t params;
    uint8_t raw[8];
} sdc_hci_cmd_le_set_event_mask_t;

/** @brief LE Read Buffer Size [v1] return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t le_acl_data_packet_length;
    uint8_t total_num_le_acl_data_packets;
} sdc_hci_cmd_le_read_buffer_size_return_t;

/** @brief LE Read Local Supported Features Page 0 return parameter(s). */
typedef union __PACKED __ALIGN(1)
{
    sdc_hci_le_le_features_t params;
    uint8_t raw[8];
} sdc_hci_cmd_le_read_local_supported_features_return_t;

/** @brief LE Set Random Address command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t random_address[6];
} sdc_hci_cmd_le_set_random_address_t;

/** @brief LE Set Advertising Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t adv_interval_min;
    uint16_t adv_interval_max;
    uint8_t adv_type;
    uint8_t own_address_type;
    uint8_t peer_address_type;
    uint8_t peer_address[6];
    uint8_t adv_channel_map;
    uint8_t adv_filter_policy;
} sdc_hci_cmd_le_set_adv_params_t;

/** @brief LE Read Advertising Physical Channel Tx Power return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    int8_t tx_power_level;
} sdc_hci_cmd_le_read_adv_physical_channel_tx_power_return_t;

/** @brief LE Set Advertising Data command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_data_length;
    uint8_t adv_data[31];
} sdc_hci_cmd_le_set_adv_data_t;

/** @brief LE Set Scan Response Data command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t scan_response_data_length;
    uint8_t scan_response_data[31];
} sdc_hci_cmd_le_set_scan_response_data_t;

/** @brief LE Set Advertising Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_enable;
} sdc_hci_cmd_le_set_adv_enable_t;

/** @brief LE Set Scan Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t le_scan_type;
    uint16_t le_scan_interval;
    uint16_t le_scan_window;
    uint8_t own_address_type;
    uint8_t scanning_filter_policy;
} sdc_hci_cmd_le_set_scan_params_t;

/** @brief LE Set Scan Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t le_scan_enable;
    uint8_t filter_duplicates;
} sdc_hci_cmd_le_set_scan_enable_t;

/** @brief LE Create Connection command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t le_scan_interval;
    uint16_t le_scan_window;
    uint8_t initiator_filter_policy;
    uint8_t peer_address_type;
    uint8_t peer_address[6];
    uint8_t own_address_type;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t max_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_length;
    uint16_t max_ce_length;
} sdc_hci_cmd_le_create_conn_t;

/** @brief LE Read Filter Accept List Size return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t filter_accept_list_size;
} sdc_hci_cmd_le_read_filter_accept_list_size_return_t;

/** @brief LE Add Device To Filter Accept List command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t address_type;
    uint8_t address[6];
} sdc_hci_cmd_le_add_device_to_filter_accept_list_t;

/** @brief LE Remove Device From Filter Accept List command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t address_type;
    uint8_t address[6];
} sdc_hci_cmd_le_remove_device_from_filter_accept_list_t;

/** @brief LE Connection Update command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t max_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_length;
    uint16_t max_ce_length;
} sdc_hci_cmd_le_conn_update_t;

/** @brief LE Set Host Channel Classification command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t channel_map[5];
} sdc_hci_cmd_le_set_host_channel_classification_t;

/** @brief LE Read Channel Map command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_read_channel_map_t;

/** @brief LE Read Channel Map return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t channel_map[5];
} sdc_hci_cmd_le_read_channel_map_return_t;

/** @brief LE Read Remote Features Page 0 command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_read_remote_features_t;

/** @brief LE Encrypt command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t key[16];
    uint8_t plaintext_data[16];
} sdc_hci_cmd_le_encrypt_t;

/** @brief LE Encrypt return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t encrypted_data[16];
} sdc_hci_cmd_le_encrypt_return_t;

/** @brief LE Rand return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint64_t random_number;
} sdc_hci_cmd_le_rand_return_t;

/** @brief LE Enable Encryption command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t random_number[8];
    uint8_t encrypted_diversifier[2];
    uint8_t long_term_key[16];
} sdc_hci_cmd_le_enable_encryption_t;

/** @brief LE Long Term Key Request Reply command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t long_term_key[16];
} sdc_hci_cmd_le_long_term_key_request_reply_t;

/** @brief LE Long Term Key Request Reply return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_long_term_key_request_reply_return_t;

/** @brief LE Long Term Key Request Negative Reply command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_long_term_key_request_negative_reply_t;

/** @brief LE Long Term Key Request Negative Reply return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_long_term_key_request_negative_reply_return_t;

/** @brief LE Read Supported States return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    /** @brief See the table in Core Specification, Vol 4, Part E, Section 7.8.27. */
    uint8_t le_states[8];
} sdc_hci_cmd_le_read_supported_states_return_t;

/** @brief LE Receiver Test [v1] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t rx_channel;
} sdc_hci_cmd_le_receiver_test_v1_t;

/** @brief LE Transmitter Test [v1] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t tx_channel;
    uint8_t test_data_length;
    uint8_t packet_payload;
} sdc_hci_cmd_le_transmitter_test_v1_t;

/** @brief LE Test End return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t num_packets;
} sdc_hci_cmd_le_test_end_return_t;

/** @brief LE Set Data Length command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t tx_octets;
    uint16_t tx_time;
} sdc_hci_cmd_le_set_data_length_t;

/** @brief LE Set Data Length return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_set_data_length_return_t;

/** @brief LE Read Suggested Default Data Length return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t suggested_max_tx_octets;
    uint16_t suggested_max_tx_time;
} sdc_hci_cmd_le_read_suggested_default_data_length_return_t;

/** @brief LE Write Suggested Default Data Length command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t suggested_max_tx_octets;
    uint16_t suggested_max_tx_time;
} sdc_hci_cmd_le_write_suggested_default_data_length_t;

/** @brief LE Add Device To Resolving List command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t peer_identity_address_type;
    uint8_t peer_identity_address[6];
    uint8_t peer_irk[16];
    uint8_t local_irk[16];
} sdc_hci_cmd_le_add_device_to_resolving_list_t;

/** @brief LE Remove Device From Resolving List command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t peer_identity_address_type;
    uint8_t peer_identity_address[6];
} sdc_hci_cmd_le_remove_device_from_resolving_list_t;

/** @brief LE Read Resolving List Size return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t resolving_list_size;
} sdc_hci_cmd_le_read_resolving_list_size_return_t;

/** @brief LE Set Address Resolution Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t address_resolution_enable;
} sdc_hci_cmd_le_set_address_resolution_enable_t;

/** @brief LE Set Resolvable Private Address Timeout [v1] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t rpa_timeout;
} sdc_hci_cmd_le_set_resolvable_private_address_timeout_t;

/** @brief LE Read Maximum Data Length return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t supported_max_tx_octets;
    uint16_t supported_max_tx_time;
    uint16_t supported_max_rx_octets;
    uint16_t supported_max_rx_time;
} sdc_hci_cmd_le_read_max_data_length_return_t;

/** @brief LE Read PHY command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_read_phy_t;

/** @brief LE Read PHY return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t tx_phy;
    uint8_t rx_phy;
} sdc_hci_cmd_le_read_phy_return_t;

/** @brief LE Set Default PHY command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
} sdc_hci_cmd_le_set_default_phy_t;

/** @brief LE Set PHY command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
    uint16_t phy_options;
} sdc_hci_cmd_le_set_phy_t;

/** @brief LE Receiver Test [v2] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t rx_channel;
    uint8_t phy;
    uint8_t modulation_index;
} sdc_hci_cmd_le_receiver_test_v2_t;

/** @brief LE Transmitter Test [v2] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t tx_channel;
    uint8_t test_data_length;
    uint8_t packet_payload;
    uint8_t phy;
} sdc_hci_cmd_le_transmitter_test_v2_t;

/** @brief LE Set Advertising Set Random Address command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t random_address[6];
} sdc_hci_cmd_le_set_adv_set_random_address_t;

/** @brief LE Set Extended Advertising Parameters [v1] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    union __PACKED __ALIGN(1) {
        sdc_hci_le_adv_event_properties_params_t params;
        uint8_t raw[2];
    } adv_event_properties;
    uint32_t primary_adv_interval_min : 24;
    uint32_t primary_adv_interval_max : 24;
    uint8_t primary_adv_channel_map;
    uint8_t own_address_type;
    uint8_t peer_address_type;
    uint8_t peer_address[6];
    uint8_t adv_filter_policy;
    int8_t adv_tx_power;
    uint8_t primary_adv_phy;
    uint8_t secondary_adv_max_skip;
    uint8_t secondary_adv_phy;
    uint8_t adv_sid;
    uint8_t scan_request_notification_enable;
} sdc_hci_cmd_le_set_ext_adv_params_t;

/** @brief LE Set Extended Advertising Parameters [v1] return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    int8_t selected_tx_power;
} sdc_hci_cmd_le_set_ext_adv_params_return_t;

/** @brief LE Set Extended Advertising Data command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t operation;
    uint8_t fragment_preference;
    uint8_t adv_data_length;
    uint8_t adv_data[];
} sdc_hci_cmd_le_set_ext_adv_data_t;

/** @brief LE Set Extended Scan Response Data command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t operation;
    uint8_t fragment_preference;
    uint8_t scan_response_data_length;
    uint8_t scan_response_data[];
} sdc_hci_cmd_le_set_ext_scan_response_data_t;

/** @brief LE Set Extended Advertising Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t enable;
    uint8_t num_sets;
    sdc_hci_le_set_ext_adv_enable_array_params_t array_params[];
} sdc_hci_cmd_le_set_ext_adv_enable_t;

/** @brief LE Read Maximum Advertising Data Length return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t max_adv_data_length;
} sdc_hci_cmd_le_read_max_adv_data_length_return_t;

/** @brief LE Read Number of Supported Advertising Sets return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t num_supported_adv_sets;
} sdc_hci_cmd_le_read_number_of_supported_adv_sets_return_t;

/** @brief LE Remove Advertising Set command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
} sdc_hci_cmd_le_remove_adv_set_t;

/** @brief LE Set Periodic Advertising Parameters [v1] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint16_t periodic_adv_interval_min;
    uint16_t periodic_adv_interval_max;
    uint16_t periodic_adv_properties;
} sdc_hci_cmd_le_set_periodic_adv_params_t;

/** @brief LE Set Periodic Advertising Data command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t operation;
    uint8_t adv_data_length;
    uint8_t adv_data[];
} sdc_hci_cmd_le_set_periodic_adv_data_t;

/** @brief LE Set Periodic Advertising Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    union __PACKED __ALIGN(1) {
        sdc_hci_le_set_periodic_adv_enable_params_t params;
        uint8_t raw[1];
    } enable;
    uint8_t adv_handle;
} sdc_hci_cmd_le_set_periodic_adv_enable_t;

/** @brief LE Set Extended Scan Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t own_address_type;
    uint8_t scanning_filter_policy;
    uint8_t scanning_phys;
    sdc_hci_le_set_ext_scan_params_array_params_t array_params[];
} sdc_hci_cmd_le_set_ext_scan_params_t;

/** @brief LE Set Extended Scan Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t enable;
    uint8_t filter_duplicates;
    uint16_t duration;
    uint16_t period;
} sdc_hci_cmd_le_set_ext_scan_enable_t;

/** @brief LE Extended Create Connection [v1] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t initiator_filter_policy;
    uint8_t own_address_type;
    uint8_t peer_address_type;
    uint8_t peer_address[6];
    uint8_t initiating_phys;
    sdc_hci_le_ext_create_conn_array_params_t array_params[];
} sdc_hci_cmd_le_ext_create_conn_t;

/** @brief LE Periodic Advertising Create Sync command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    union __PACKED __ALIGN(1) {
        sdc_hci_le_periodic_adv_create_sync_options_params_t params;
        uint8_t raw[1];
    } options;
    uint8_t adv_sid;
    uint8_t adv_address_type;
    uint8_t adv_address[6];
    uint16_t skip;
    uint16_t sync_timeout;
    uint8_t sync_cte_type;
} sdc_hci_cmd_le_periodic_adv_create_sync_t;

/** @brief LE Periodic Advertising Terminate Sync command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t sync_handle;
} sdc_hci_cmd_le_periodic_adv_terminate_sync_t;

/** @brief LE Add Device To Periodic Advertiser List command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_address_type;
    uint8_t adv_address[6];
    uint8_t adv_sid;
} sdc_hci_cmd_le_add_device_to_periodic_adv_list_t;

/** @brief LE Remove Device From Periodic Advertiser List command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_address_type;
    uint8_t adv_address[6];
    uint8_t adv_sid;
} sdc_hci_cmd_le_remove_device_from_periodic_adv_list_t;

/** @brief LE Read Periodic Advertiser List Size return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t periodic_adv_list_size;
} sdc_hci_cmd_le_read_periodic_adv_list_size_return_t;

/** @brief LE Read Transmit Power return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    int8_t min_tx_power;
    int8_t max_tx_power;
} sdc_hci_cmd_le_read_transmit_power_return_t;

/** @brief LE Read RF Path Compensation return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    int16_t rf_tx_path_compensation_value;
    int16_t rf_rx_path_compensation_value;
} sdc_hci_cmd_le_read_rf_path_compensation_return_t;

/** @brief LE Write RF Path Compensation command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    int16_t rf_tx_path_compensation_value;
    int16_t rf_rx_path_compensation_value;
} sdc_hci_cmd_le_write_rf_path_compensation_t;

/** @brief LE Set Privacy Mode command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t peer_identity_address_type;
    uint8_t peer_identity_address[6];
    uint8_t privacy_mode;
} sdc_hci_cmd_le_set_privacy_mode_t;

/** @brief LE Receiver Test [v3] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t rx_channel;
    uint8_t phy;
    uint8_t modulation_index;
    uint8_t expected_cte_length;
    uint8_t expected_cte_type;
    uint8_t slot_durations;
    uint8_t switching_pattern_length;
    uint8_t antenna_ids[];
} sdc_hci_cmd_le_receiver_test_v3_t;

/** @brief LE Transmitter Test [v3] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t tx_channel;
    uint8_t test_data_length;
    uint8_t packet_payload;
    uint8_t phy;
    uint8_t cte_length;
    uint8_t cte_type;
    uint8_t switching_pattern_length;
    uint8_t antenna_ids[];
} sdc_hci_cmd_le_transmitter_test_v3_t;

/** @brief LE Set Connectionless CTE Transmit Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t cte_length;
    uint8_t cte_type;
    uint8_t cte_count;
    uint8_t switching_pattern_length;
    sdc_hci_le_set_connless_cte_transmit_params_array_params_t array_params[];
} sdc_hci_cmd_le_set_connless_cte_transmit_params_t;

/** @brief LE Set Connectionless CTE Transmit Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t cte_enable;
} sdc_hci_cmd_le_set_connless_cte_transmit_enable_t;

/** @brief LE Set Connection CTE Transmit Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t cte_types;
    uint8_t switching_pattern_length;
    sdc_hci_le_set_conn_cte_transmit_params_array_params_t array_params[];
} sdc_hci_cmd_le_set_conn_cte_transmit_params_t;

/** @brief LE Set Connection CTE Transmit Parameters return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_set_conn_cte_transmit_params_return_t;

/** @brief LE Connection CTE Response Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t enable;
} sdc_hci_cmd_le_conn_cte_response_enable_t;

/** @brief LE Connection CTE Response Enable return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_conn_cte_response_enable_return_t;

/** @brief LE Read Antenna Information return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t supported_switching_sampling_rates;
    uint8_t num_antennae;
    uint8_t max_switching_pattern_length;
    uint8_t max_cte_length;
} sdc_hci_cmd_le_read_antenna_information_return_t;

/** @brief LE Set Periodic Advertising Receive Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t sync_handle;
    union __PACKED __ALIGN(1) {
        sdc_hci_le_set_periodic_adv_receive_enable_params_t params;
        uint8_t raw[1];
    } enable;
} sdc_hci_cmd_le_set_periodic_adv_receive_enable_t;

/** @brief LE Periodic Advertising Sync Transfer command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t service_data;
    uint16_t sync_handle;
} sdc_hci_cmd_le_periodic_adv_sync_transfer_t;

/** @brief LE Periodic Advertising Sync Transfer return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_periodic_adv_sync_transfer_return_t;

/** @brief LE Periodic Advertising Set Info Transfer command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t service_data;
    uint8_t adv_handle;
} sdc_hci_cmd_le_periodic_adv_set_info_transfer_t;

/** @brief LE Periodic Advertising Set Info Transfer return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_periodic_adv_set_info_transfer_return_t;

/** @brief LE Set Periodic Advertising Sync Transfer Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t mode;
    uint16_t skip;
    uint16_t sync_timeout;
    uint8_t cte_type;
} sdc_hci_cmd_le_set_periodic_adv_sync_transfer_params_t;

/** @brief LE Set Periodic Advertising Sync Transfer Parameters return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_set_periodic_adv_sync_transfer_params_return_t;

/** @brief LE Set Default Periodic Advertising Sync Transfer Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t mode;
    uint16_t skip;
    uint16_t sync_timeout;
    uint8_t cte_type;
} sdc_hci_cmd_le_set_default_periodic_adv_sync_transfer_params_t;

/** @brief LE Read Buffer Size [v2] return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t le_acl_data_packet_length;
    uint8_t total_num_le_acl_data_packets;
    uint16_t iso_data_packet_length;
    uint8_t total_num_iso_data_packets;
} sdc_hci_cmd_le_read_buffer_size_v2_return_t;

/** @brief LE Read ISO TX Sync command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_read_iso_tx_sync_t;

/** @brief LE Read ISO TX Sync return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t packet_sequence_number;
    uint32_t tx_time_stamp;
    uint32_t time_offset : 24;
} sdc_hci_cmd_le_read_iso_tx_sync_return_t;

/** @brief LE Set CIG Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cig_id;
    uint32_t sdu_interval_c_to_p : 24;
    uint32_t sdu_interval_p_to_c : 24;
    uint8_t worst_case_sca;
    uint8_t packing;
    uint8_t framing;
    uint16_t max_transport_latency_c_to_p;
    uint16_t max_transport_latency_p_to_c;
    uint8_t cis_count;
    sdc_hci_le_set_cig_params_array_params_t array_params[];
} sdc_hci_cmd_le_set_cig_params_t;

/** @brief LE Set CIG Parameters return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cig_id;
    uint8_t cis_count;
    sdc_hci_le_set_cig_params_output_array_params_t array_params[];
} sdc_hci_cmd_le_set_cig_params_return_t;

/** @brief LE Set CIG Parameters Test command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cig_id;
    uint32_t sdu_interval_c_to_p : 24;
    uint32_t sdu_interval_p_to_c : 24;
    uint8_t ft_c_to_p;
    uint8_t ft_p_to_c;
    uint16_t iso_interval;
    uint8_t worst_case_sca;
    uint8_t packing;
    uint8_t framing;
    uint8_t cis_count;
    sdc_hci_le_set_cig_params_test_array_params_t array_params[];
} sdc_hci_cmd_le_set_cig_params_test_t;

/** @brief LE Set CIG Parameters Test return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cig_id;
    uint8_t cis_count;
    sdc_hci_le_set_cig_params_test_output_array_params_t array_params[];
} sdc_hci_cmd_le_set_cig_params_test_return_t;

/** @brief LE Create CIS command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cis_count;
    sdc_hci_le_create_cis_array_params_t array_params[];
} sdc_hci_cmd_le_create_cis_t;

/** @brief LE Remove CIG command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cig_id;
} sdc_hci_cmd_le_remove_cig_t;

/** @brief LE Remove CIG return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t cig_id;
} sdc_hci_cmd_le_remove_cig_return_t;

/** @brief LE Accept CIS Request command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_accept_cis_request_t;

/** @brief LE Reject CIS Request command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t reason;
} sdc_hci_cmd_le_reject_cis_request_t;

/** @brief LE Reject CIS Request return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_reject_cis_request_return_t;

/** @brief LE Create BIG command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t big_handle;
    uint8_t adv_handle;
    uint8_t num_bis;
    uint32_t sdu_interval : 24;
    uint16_t max_sdu;
    uint16_t max_transport_latency;
    uint8_t rtn;
    uint8_t phy;
    uint8_t packing;
    uint8_t framing;
    uint8_t encryption;
    uint8_t broadcast_code[16];
} sdc_hci_cmd_le_create_big_t;

/** @brief LE Create BIG Test command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t big_handle;
    uint8_t adv_handle;
    uint8_t num_bis;
    uint32_t sdu_interval : 24;
    uint16_t iso_interval;
    uint8_t nse;
    uint16_t max_sdu;
    uint16_t max_pdu;
    uint8_t phy;
    uint8_t packing;
    uint8_t framing;
    uint8_t bn;
    uint8_t irc;
    uint8_t pto;
    uint8_t encryption;
    uint8_t broadcast_code[16];
} sdc_hci_cmd_le_create_big_test_t;

/** @brief LE Terminate BIG command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t big_handle;
    uint8_t reason;
} sdc_hci_cmd_le_terminate_big_t;

/** @brief LE BIG Create Sync command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t big_handle;
    uint16_t sync_handle;
    uint8_t encryption;
    uint8_t broadcast_code[16];
    uint8_t mse;
    uint16_t big_sync_timeout;
    uint8_t num_bis;
    sdc_hci_le_big_create_sync_array_params_t array_params[];
} sdc_hci_cmd_le_big_create_sync_t;

/** @brief LE BIG Terminate Sync command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t big_handle;
} sdc_hci_cmd_le_big_terminate_sync_t;

/** @brief LE BIG Terminate Sync return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t big_handle;
} sdc_hci_cmd_le_big_terminate_sync_return_t;

/** @brief LE Request Peer SCA command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_request_peer_sca_t;

/** @brief LE Setup ISO Data Path command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t data_path_direction;
    uint8_t data_path_id;
    uint8_t codec_id[5];
    uint32_t controller_delay : 24;
    uint8_t codec_config_length;
    uint8_t codec_config[];
} sdc_hci_cmd_le_setup_iso_data_path_t;

/** @brief LE Setup ISO Data Path return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_setup_iso_data_path_return_t;

/** @brief LE Remove ISO Data Path command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t data_path_direction;
} sdc_hci_cmd_le_remove_iso_data_path_t;

/** @brief LE Remove ISO Data Path return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_remove_iso_data_path_return_t;

/** @brief LE ISO Transmit Test command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t payload_type;
} sdc_hci_cmd_le_iso_transmit_test_t;

/** @brief LE ISO Transmit Test return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_iso_transmit_test_return_t;

/** @brief LE ISO Receive Test command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t payload_type;
} sdc_hci_cmd_le_iso_receive_test_t;

/** @brief LE ISO Receive Test return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_iso_receive_test_return_t;

/** @brief LE ISO Read Test Counters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_iso_read_test_counters_t;

/** @brief LE ISO Read Test Counters return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint32_t received_sdu_count;
    uint32_t missed_sdu_count;
    uint32_t failed_sdu_count;
} sdc_hci_cmd_le_iso_read_test_counters_return_t;

/** @brief LE ISO Test End command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_iso_test_end_t;

/** @brief LE ISO Test End return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint32_t received_sdu_count;
    uint32_t missed_sdu_count;
    uint32_t failed_sdu_count;
} sdc_hci_cmd_le_iso_test_end_return_t;

/** @brief LE Set Host Feature [v1] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t bit_number;
    uint8_t bit_value;
} sdc_hci_cmd_le_set_host_feature_t;

/** @brief LE Read ISO Link Quality command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_read_iso_link_quality_t;

/** @brief LE Read ISO Link Quality return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint32_t tx_unacked_packets;
    uint32_t tx_flushed_packets;
    uint32_t tx_last_subevent_packets;
    uint32_t retransmitted_packets;
    uint32_t crc_error_packets;
    uint32_t rx_unreceived_packets;
    uint32_t duplicate_packets;
} sdc_hci_cmd_le_read_iso_link_quality_return_t;

/** @brief LE Enhanced Read Transmit Power Level command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t phy;
} sdc_hci_cmd_le_enhanced_read_transmit_power_level_t;

/** @brief LE Enhanced Read Transmit Power Level return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t phy;
    int8_t current_tx_power_level;
    int8_t max_tx_power_level;
} sdc_hci_cmd_le_enhanced_read_transmit_power_level_return_t;

/** @brief LE Read Remote Transmit Power Level command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t phy;
} sdc_hci_cmd_le_read_remote_transmit_power_level_t;

/** @brief LE Set Path Loss Reporting Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t high_threshold;
    uint8_t high_hysteresis;
    uint8_t low_threshold;
    uint8_t low_hysteresis;
    uint16_t min_time_spent;
} sdc_hci_cmd_le_set_path_loss_reporting_params_t;

/** @brief LE Set Path Loss Reporting Parameters return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_set_path_loss_reporting_params_return_t;

/** @brief LE Set Path Loss Reporting Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t enable;
} sdc_hci_cmd_le_set_path_loss_reporting_enable_t;

/** @brief LE Set Path Loss Reporting Enable return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_set_path_loss_reporting_enable_return_t;

/** @brief LE Set Transmit Power Reporting Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t local_enable;
    uint8_t remote_enable;
} sdc_hci_cmd_le_set_transmit_power_reporting_enable_t;

/** @brief LE Set Transmit Power Reporting Enable return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_set_transmit_power_reporting_enable_return_t;

/** @brief LE Transmitter Test [v4] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t tx_channel;
    uint8_t test_data_length;
    uint8_t packet_payload;
    uint8_t phy;
    uint8_t cte_length;
    uint8_t cte_type;
    uint8_t switching_pattern_length;
    uint8_t antenna_ids_and_remaining_parameters[];
} sdc_hci_cmd_le_transmitter_test_v4_t;

/** @brief LE Set Data Related Address Changes command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    union __PACKED __ALIGN(1) {
        sdc_hci_le_set_data_related_address_changes_reasons_params_t params;
        uint8_t raw[1];
    } change_reasons;
} sdc_hci_cmd_le_set_data_related_address_changes_t;

/** @brief LE Set Default Subrate command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t subrate_min;
    uint16_t subrate_max;
    uint16_t max_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
} sdc_hci_cmd_le_set_default_subrate_t;

/** @brief LE Subrate Request command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t subrate_min;
    uint16_t subrate_max;
    uint16_t max_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
} sdc_hci_cmd_le_subrate_request_t;

/** @brief LE Set Extended Advertising Parameters [v2] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    union __PACKED __ALIGN(1) {
        sdc_hci_le_adv_event_properties_params_t params;
        uint8_t raw[2];
    } adv_event_properties;
    uint32_t primary_adv_interval_min : 24;
    uint32_t primary_adv_interval_max : 24;
    uint8_t primary_adv_channel_map;
    uint8_t own_address_type;
    uint8_t peer_address_type;
    uint8_t peer_address[6];
    uint8_t adv_filter_policy;
    int8_t adv_tx_power;
    uint8_t primary_adv_phy;
    uint8_t secondary_adv_max_skip;
    uint8_t secondary_adv_phy;
    uint8_t adv_sid;
    uint8_t scan_request_notification_enable;
    uint8_t primary_adv_phy_options;
    uint8_t secondary_adv_phy_options;
} sdc_hci_cmd_le_set_ext_adv_params_v2_t;

/** @brief LE Set Extended Advertising Parameters [v2] return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    int8_t selected_tx_power;
} sdc_hci_cmd_le_set_ext_adv_params_v2_return_t;

/** @brief LE Set Periodic Advertising Subevent Data command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t num_subevents;
    /** C does not allow array of variable sized arrays, otherwise it should be @ref
    sdc_hci_le_set_periodic_adv_subevent_data_array_params_t array_params[]; */
    uint8_t array_params[];
} sdc_hci_cmd_le_set_periodic_adv_subevent_data_t;

/** @brief LE Set Periodic Advertising Subevent Data return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
} sdc_hci_cmd_le_set_periodic_adv_subevent_data_return_t;

/** @brief LE Set Periodic Advertising Response Data command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t sync_handle;
    uint16_t request_event;
    uint8_t request_subevent;
    uint8_t response_subevent;
    uint8_t response_slot;
    uint8_t response_data_length;
    uint8_t response_data[];
} sdc_hci_cmd_le_set_periodic_adv_response_data_t;

/** @brief LE Set Periodic Advertising Response Data return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t sync_handle;
} sdc_hci_cmd_le_set_periodic_adv_response_data_return_t;

/** @brief LE Set Periodic Sync Subevent command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t sync_handle;
    uint16_t periodic_adv_properties;
    uint8_t num_subevents;
    uint8_t subevents[];
} sdc_hci_cmd_le_set_periodic_sync_subevent_t;

/** @brief LE Set Periodic Sync Subevent return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t sync_handle;
} sdc_hci_cmd_le_set_periodic_sync_subevent_return_t;

/** @brief LE Extended Create Connection [v2] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint8_t subevent;
    uint8_t initiator_filter_policy;
    uint8_t own_address_type;
    uint8_t peer_address_type;
    uint8_t peer_address[6];
    uint8_t initiating_phys;
    sdc_hci_le_ext_create_conn_v2_array_params_t array_params[];
} sdc_hci_cmd_le_ext_create_conn_v2_t;

/** @brief LE Set Periodic Advertising Parameters [v2] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
    uint16_t periodic_adv_interval_min;
    uint16_t periodic_adv_interval_max;
    uint16_t periodic_adv_properties;
    uint8_t num_subevents;
    uint8_t subevent_interval;
    uint8_t response_slot_delay;
    uint8_t response_slot_spacing;
    uint8_t num_response_slots;
} sdc_hci_cmd_le_set_periodic_adv_params_v2_t;

/** @brief LE Set Periodic Advertising Parameters [v2] return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t adv_handle;
} sdc_hci_cmd_le_set_periodic_adv_params_v2_return_t;

/** @brief LE Read All Local Supported Features return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t max_page;
    uint8_t le_features[248];
} sdc_hci_cmd_le_read_all_local_supported_features_return_t;

/** @brief LE Read All Remote Features command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t pages_requested;
} sdc_hci_cmd_le_read_all_remote_features_t;

/** @brief LE CS Read Local Supported Capabilities return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t num_config_supported;
    uint16_t max_consecutive_procedures_supported;
    uint8_t num_antennas_supported;
    uint8_t max_antenna_paths_supported;
    uint8_t roles_supported;
    uint8_t modes_supported;
    uint8_t rtt_capability;
    uint8_t rtt_aa_only_n;
    uint8_t rtt_sounding_n;
    uint8_t rtt_random_payload_n;
    uint16_t nadm_sounding_capability;
    uint16_t nadm_random_capability;
    uint8_t cs_sync_phys_supported;
    uint16_t subfeatures_supported;
    uint16_t t_ip1_times_supported;
    uint16_t t_ip2_times_supported;
    uint16_t t_fcs_times_supported;
    uint16_t t_pm_times_supported;
    uint8_t t_sw_time_supported;
    uint8_t tx_snr_capability;
} sdc_hci_cmd_le_cs_read_local_supported_capabilities_return_t;

/** @brief LE CS Read Remote Supported Capabilities command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_cs_read_remote_supported_capabilities_t;

/** @brief LE CS Write Cached Remote Supported Capabilities command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t num_config_supported;
    uint16_t max_consecutive_procedures_supported;
    uint8_t num_antennas_supported;
    uint8_t max_antenna_paths_supported;
    uint8_t roles_supported;
    uint8_t modes_supported;
    uint8_t rtt_capability;
    uint8_t rtt_aa_only_n;
    uint8_t rtt_sounding_n;
    uint8_t rtt_random_payload_n;
    uint16_t nadm_sounding_capability;
    uint16_t nadm_random_capability;
    uint8_t cs_sync_phys_supported;
    uint16_t subfeatures_supported;
    uint16_t t_ip1_times_supported;
    uint16_t t_ip2_times_supported;
    uint16_t t_fcs_times_supported;
    uint16_t t_pm_times_supported;
    uint8_t t_sw_time_supported;
    uint8_t tx_snr_capability;
} sdc_hci_cmd_le_cs_write_cached_remote_supported_capabilities_t;

/** @brief LE CS Write Cached Remote Supported Capabilities return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_cs_write_cached_remote_supported_capabilities_return_t;

/** @brief LE CS Security Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_cs_security_enable_t;

/** @brief LE CS Set Default Settings command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t role_enable;
    uint8_t cs_sync_antenna_selection;
    int8_t max_tx_power;
} sdc_hci_cmd_le_cs_set_default_settings_t;

/** @brief LE CS Set Default Settings return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_cs_set_default_settings_return_t;

/** @brief LE CS Read Remote FAE Table command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_cs_read_remote_fae_table_t;

/** @brief LE CS Write Cached Remote FAE Table command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t remote_fae_table[72];
} sdc_hci_cmd_le_cs_write_cached_remote_fae_table_t;

/** @brief LE CS Write Cached Remote FAE Table return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_cs_write_cached_remote_fae_table_return_t;

/** @brief LE CS Create Config command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t config_id;
    uint8_t create_context;
    uint8_t main_mode_type;
    uint8_t sub_mode_type;
    uint8_t min_main_mode_steps;
    uint8_t max_main_mode_steps;
    uint8_t main_mode_repetition;
    uint8_t mode_0_steps;
    uint8_t role;
    uint8_t rtt_type;
    uint8_t cs_sync_phy;
    uint8_t channel_map[10];
    uint8_t channel_map_repetition;
    uint8_t channel_selection_type;
    uint8_t ch3c_shape;
    uint8_t ch3c_jump;
    uint8_t reserved;
} sdc_hci_cmd_le_cs_create_config_t;

/** @brief LE CS Remove Config command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t config_id;
} sdc_hci_cmd_le_cs_remove_config_t;

/** @brief LE CS Set Channel Classification command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t channel_classification[10];
} sdc_hci_cmd_le_cs_set_channel_classification_t;

/** @brief LE CS Set Procedure Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t config_id;
    uint16_t max_procedure_len;
    uint16_t min_procedure_interval;
    uint16_t max_procedure_interval;
    uint16_t max_procedure_count;
    uint32_t min_subevent_len : 24;
    uint32_t max_subevent_len : 24;
    uint8_t tone_antenna_config_selection;
    uint8_t phy;
    int8_t tx_power_delta;
    uint8_t preferred_peer_antenna;
    uint8_t snr_control_initiator;
    uint8_t snr_control_reflector;
} sdc_hci_cmd_le_cs_set_procedure_params_t;

/** @brief LE CS Set Procedure Parameters return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_le_cs_set_procedure_params_return_t;

/** @brief LE CS Procedure Enable command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t config_id;
    uint8_t enable;
} sdc_hci_cmd_le_cs_procedure_enable_t;

/** @brief LE CS Test command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t main_mode_type;
    uint8_t sub_mode_type;
    uint8_t main_mode_repetition;
    uint8_t mode_0_steps;
    uint8_t role;
    uint8_t rtt_type;
    uint8_t cs_sync_phy;
    uint8_t cs_sync_antenna_selection;
    uint32_t subevent_len : 24;
    uint16_t subevent_interval;
    uint8_t max_num_subevents;
    int8_t transmit_power_level;
    uint8_t t_ip1_time;
    uint8_t t_ip2_time;
    uint8_t t_fcs_time;
    uint8_t t_pm_time;
    uint8_t t_sw_time;
    uint8_t tone_antenna_config_selection;
    uint8_t reserved;
    uint8_t snr_control_initiator;
    uint8_t snr_control_reflector;
    uint16_t drbg_nonce;
    uint8_t channel_map_repetition;
    uint16_t override_config;
    uint8_t override_params_length;
    uint8_t override_params_data[];
} sdc_hci_cmd_le_cs_test_t;

/** @brief LE Set Host Feature [v2] command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t bit_number;
    uint8_t bit_value;
} sdc_hci_cmd_le_set_host_feature_v2_t;

/** @brief LE Frame Space Update command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t frame_space_min;
    uint16_t frame_space_max;
    uint8_t phys;
    uint16_t spacing_types;
} sdc_hci_cmd_le_frame_space_update_t;

/** @brief LE Connection Rate Request command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t subrate_min;
    uint16_t subrate_max;
    uint16_t max_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
    uint16_t min_ce_length;
    uint16_t max_ce_length;
} sdc_hci_cmd_le_conn_rate_request_t;

/** @brief LE Set Default Rate Parameters command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t subrate_min;
    uint16_t subrate_max;
    uint16_t max_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
    uint16_t min_ce_length;
    uint16_t max_ce_length;
} sdc_hci_cmd_le_set_default_rate_params_t;

/** @brief LE Read Minimum Supported Connection Interval return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t min_supported_conn_interval;
    uint8_t num_groups;
    sdc_hci_le_read_min_supported_conn_interval_group_t groups[];
} sdc_hci_cmd_le_read_min_supported_conn_interval_return_t;

/** @} end of HCI_COMMAND_PARAMETERS */

/**
 * @defgroup HCI_VS_API API
 * @{
 */
/** @brief LE Set Event Mask.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.1
 *
 * This command is used to control which LE events are generated by the HCI for the
 * Host. If the bit in the LE_Event_Mask is set to a one, then the event associated with that
 * bit will be enabled. The event mask allows the Host to control which events will interrupt
 * it.
 *
 * The Controller shall ignore those bits which are reserved for future use or represent
 * events which it does not support. If the Host sets any of these bits to 1, the Controller
 * shall act as if they were set to 0.
 *
 * For LE events to be generated, the LE Meta event bit in the Event_Mask shall also be
 * set. If that bit is not set, then LE events shall not be generated, regardless of how the
 * LE_Event_Mask is set.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Event_Mask command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_event_mask(const sdc_hci_cmd_le_set_event_mask_t * p_params);

/** @brief LE Read Buffer Size [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.2
 *
 * This command is used to read the maximum size of the data portion of ACL data
 * packets and isochronous data packets sent from the Host to the Controller. The Host
 * shall fragment the data transmitted to the Controller according to these values so that
 * the HCI ACL Data packets and HCI ISO Data packets will contain data up to this size
 * (“data” includes optional fields in the HCI ISO Data packet, such as ISO_SDU_Length).
 * The HCI_LE_Read_Buffer_Size command also returns the total number of HCI LE ACL
 * Data packets and isochronous data packets that can be stored in the data buffers of the
 * Controller. The HCI_LE_Read_Buffer_Size command shall be issued by the Host before
 * it sends any data to an LE Controller (see Section 4.1.1). If the Controller supports HCI
 * ISO Data packets, it shall return non-zero values for the ISO_Data_Packet_Length and
 * Total_Num_ISO_Data_Packets parameters.
 *
 * If the Controller returns a length value of zero for ACL data packets, the Host shall use
 * the HCI_Read_Buffer_Size command to determine the size of the data buffers (shared
 * between BR/EDR and LE transports).
 *
 * Note: Both the HCI_Read_Buffer_Size command and the HCI_LE_Read_Buffer_Size
 * command may return buffer length and number of packets parameter values that are
 * nonzero. This allows a Controller to offer different buffers and number of buffers for
 * BR/EDR data packets and LE data packets.
 *
 * The LE_ACL_Data_Packet_Length parameter shall be used to determine the maximum
 * size of the L2CAP PDU fragments that are contained in ACL data packets, and which
 * are transferred from the Host to the Controller to be broken up into packets by the Link
 * Layer. The Total_Num_LE_ACL_Data_Packets parameter contains the total number of
 * HCI ACL Data packets that can be stored in the data buffers of the Controller. The Host
 * determines how to divide the buffers between different connection handles.
 * The ISO_Data_Packet_Length parameter shall be used to determine the maximum
 * size of the SDU fragments that are contained in isochronous data packets, and which
 * are transferred from the Host to the Controller. The Total_Num_ISO_Data_Packets
 * parameter contains the total number of isochronous data packets that can be stored
 * in the data buffers of the Controller. The Host determines how to divide the buffers
 * between different connection handle(s).
 *
 * Note: The LE_ACL_Data_Packet_Length and ISO_Data_Packet_Length return
 * parameters do not include the length of the HCI ACL Data packet header or the HCI
 * ISO Data packet header respectively.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Buffer_Size command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_buffer_size(sdc_hci_cmd_le_read_buffer_size_return_t * p_return);

/** @brief LE Read Local Supported Features Page 0.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.3
 *
 * This command requests page 0 of the list of the supported LE features for the
 * Controller.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Local_Supported_Features_Page_0 command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * This command was formerly called "LE Read Local Supported Features".
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_local_supported_features(sdc_hci_cmd_le_read_local_supported_features_return_t * p_return);

/** @brief LE Set Random Address.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.4
 *
 * This command is used by the Host to set the LE Random Device Address in the
 * Controller (see [Vol 6] Part B, Section 1.3).
 *
 * If this command is used to change the address, the new random address shall take
 * effect for advertising no later than the next successful HCI_LE_Set_Advertising_Enable
 * command, for scanning no later than the next successful HCI_LE_Set_Scan_Enable
 * command or HCI_LE_Set_Extended_Scan_Enable command, and for initiating or
 * creating a connection from a PAwR train no later than the next successful
 * HCI_LE_Create_Connection command or HCI_LE_Extended_Create_Connection
 * command.
 *
 * Note: If the extended advertising commands are in use, this command only affects the
 * address used for scanning and initiating. The addresses used for advertising are set by
 * the HCI_LE_Set_Advertising_Set_Random_Address command (see Section 7.8.52).
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                                 Error code
 * MC     Any of advertising (created using legacy advertising commands),           Command
 *        scanning, or creating a connection are enabled in the Controller.         Disallowed
 *                                                                                  (0x0C)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Random_Address command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_random_address(const sdc_hci_cmd_le_set_random_address_t * p_params);

/** @brief LE Set Advertising Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.5
 *
 * This command is used by the Host to set the advertising parameters.
 *
 * The Advertising_Interval_Min shall be less than or equal to the
 * Advertising_Interval_Max. The Advertising_Interval_Min and Advertising_Interval_Max
 * should not be the same value to enable the Controller to determine the best advertising
 * interval given other activities.
 *
 * For high duty cycle directed advertising, i.e. when Advertising_Type is
 * 0x01 (ADV_DIRECT_IND, high duty cycle), the Advertising_Interval_Min and
 * Advertising_Interval_Max parameters are not used and shall be ignored.
 *
 * The Advertising_Type is used to determine the packet type that is used for advertising
 * when advertising is enabled.
 *
 * Own_Address_Type parameter indicates the type of address being used in the
 * advertising packets.
 *
 * If Own_Address_Type equals 0x02 or 0x03, the Peer_Address parameter contains
 * the peer’s Identity Address and the Peer_Address_Type parameter contains the
 * Peer’s Identity Type (i.e. 0x00 or 0x01). These parameters are used to locate the
 * corresponding local IRK in the resolving list; this IRK is used to generate the own
 * address used in the advertisement.
 *
 * If directed advertising is performed, i.e. when Advertising_Type is set to 0x01
 * (ADV_DIRECT_IND, high duty cycle) or 0x04 (ADV_DIRECT_IND, low duty cycle
 * mode), then the Peer_Address_Type and Peer_Address shall be valid.
 *
 * If Own_Address_Type equals 0x02 or 0x03, the Controller generates the peer’s
 * Resolvable Private Address using the peer’s IRK corresponding to the peer’s Identity
 * Address contained in the Peer_Address parameter and peer’s Identity Address Type
 * (i.e. 0x00 or 0x01) contained in the Peer_Address_Type parameter.
 *
 * The Advertising_Channel_Map is a bit field that indicates the advertising channel
 * indices that shall be used when transmitting advertising packets. At least one channel
 * bit shall be set in the Advertising_Channel_Map parameter.
 *
 * The Advertising_Filter_Policy parameter shall be ignored when directed advertising is
 * enabled.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                             Error code
 * MC     Advertising is enabled in the Controller.                             Command Disallowed
 *                                                                              (0x0C)
 * MC     The advertising interval range (Advertising_Interval_Min,             Unsupported
 *        Advertising_Interval_Max) does not overlap with the advertising       Feature or
 *        interval range supported by the Controller.                           Parameter Value
 *                                                                              (0x11)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Advertising_Parameters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_adv_params(const sdc_hci_cmd_le_set_adv_params_t * p_params);

/** @brief LE Read Advertising Physical Channel Tx Power.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.6
 *
 * This command is used by the Host to read the transmit power level used for LE
 * advertising physical channel packets.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Advertising_Physical_Channel_Tx_Power command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_adv_physical_channel_tx_power(sdc_hci_cmd_le_read_adv_physical_channel_tx_power_return_t * p_return);

/** @brief LE Set Advertising Data.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.7
 *
 * This command is used to set the data used in advertising packets that have a data field.
 *
 * Only the significant part of the Advertising_Data should be transmitted in the advertising
 * packets, as defined in [Vol 3] Part C, Section 11.
 *
 * If advertising is currently enabled, the Controller shall use the new data in subsequent
 * advertising events. If an advertising event is in progress when this command is issued,
 * the Controller may use the old or new data for that event. If advertising is currently
 * disabled, the data shall be kept by the Controller and used once advertising is enabled.
 *
 * The default Advertising_Data_Length shall be zero and the default Advertising_Data
 * shall be 31 all-zero octets.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Advertising_Data command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_adv_data(const sdc_hci_cmd_le_set_adv_data_t * p_params);

/** @brief LE Set Scan Response Data.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.8
 *
 * This command is used to provide data used in Scanning Packets that have a data field.
 *
 * Only the significant part of the Scan_Response_Data should be transmitted in the
 * Scanning Packets, as defined in [Vol 3] Part C, Section 11.
 *
 * If advertising is currently enabled, the Controller shall use the new data in subsequent
 * advertising events. If an advertising event is in progress when this command is issued,
 * the Controller may use the old or new data for that event. If advertising is currently
 * disabled, the data shall be kept by the Controller and used once advertising is enabled.
 *
 * The default Scan_Response_Data_Length shall be zero and the default
 * Scan_Response_Data shall be 31 all-zero octets.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Scan_Response_Data command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_scan_response_data(const sdc_hci_cmd_le_set_scan_response_data_t * p_params);

/** @brief LE Set Advertising Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.9
 *
 * This command is used to request the Controller to start or stop advertising. The
 * Controller manages the timing of advertisements as per the advertising parameters
 * given in the HCI_LE_Set_Advertising_Parameters command.
 *
 * The Controller shall continue advertising until the Host issues an
 * HCI_LE_Set_Advertising_Enable command with Advertising_Enable set to 0x00
 * (Advertising is disabled). a connection is created using the advertising, or the
 * Advertising is timed out due to high duty cycle Directed Advertising. In these cases,
 * advertising is then disabled.
 *
 * Enabling advertising when it is already enabled can cause the random address to
 * change. Disabling advertising when it is already disabled has no effect.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                                     Error code
 * R      Advertising_Enable is set to 0x01, the advertising parameters'                Invalid
 *        Own_Address_Type parameter is set to 0x00, and the device does not have a     HCI
 *        public address.                                                               Command
 *                                                                                      Parameters
 *                                                                                      (0x12)
 * MC     Advertising_Enable is set to 0x01, the advertising parameters'                Invalid
 *        Own_Address_Type parameter is set to 0x01, and the random address for the     HCI
 *        device has not been initialized using the HCI_LE_Set_Random_Address           Command
 *        command.                                                                      Parameters
 *                                                                                      (0x12)
 * R      Advertising_Enable is set to 0x01, the advertising parameters'                Invalid
 *        Own_Address_Type parameter is set to 0x02, the Controller's resolving list    HCI
 *        does not contain a matching entry, and the device does not have a public      Command
 *        address.                                                                      Parameters
 *                                                                                      (0x12)
 * MC     Advertising_Enable is set to 0x01, the advertising parameters'                Invalid
 *        Own_Address_Type parameter is set to 0x03, the Controller's resolving list    HCI
 *        does not contain a matching entry, and the random address for the device      Command
 *        has not been initialized using the HCI_LE_Set_Random_Address command.         Parameters
 *                                                                                      (0x12)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Advertising_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * If the Advertising_Type parameter is 0x01 (ADV_DIRECT_IND, high duty cycle) and
 * the directed advertising fails to create a connection, an HCI_LE_Connection_Complete
 * or HCI_LE_Enhanced_Connection_Complete event shall be generated with the Status
 * code set to Advertising Timeout (0x3C).
 *
 * If the Advertising_Type parameter is 0x00 (ADV_IND), 0x01 (ADV_DIRECT_IND, high
 * duty cycle), or 0x04 (ADV_DIRECT_IND, low duty cycle) and a connection is created,
 * an HCI_LE_Connection_Complete or HCI_LE_Enhanced_Connection_Complete event
 * shall be generated.
 *
 * Note: There is a possible race condition if the Advertising_Enable parameter is set
 * to 0x00 (Disable) and the Advertising_Type parameter is 0x00, 0x01, or 0x04. The
 * advertisements might not be stopped before a connection is created, and therefore
 * both the HCI_Command_Complete event and either an HCI_LE_Connection_Complete
 * event or an HCI_LE_Enhanced_Connection_Complete event could be generated. This
 * can also occur when high duty cycle directed advertising is timed out and this command
 * disables advertising.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_adv_enable(const sdc_hci_cmd_le_set_adv_enable_t * p_params);

/** @brief LE Set Scan Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.10
 *
 * This command is used to set the scan parameters.
 *
 * The LE_Scan_Type parameter controls the type of scan to perform.
 *
 * The LE_Scan_Interval and LE_Scan_Window parameters are recommendations from
 * the Host on how long (LE_Scan_Window) and how frequently (LE_Scan_Interval) the
 * Controller should scan (See [Vol 6] Part B, Section 4.4.3). The LE_Scan_Window
 * parameter shall always be set to a value smaller or equal to the value set for the
 * LE_Scan_Interval parameter. If they are set to the same value scanning should be run
 * continuously.
 *
 * Own_Address_Type parameter indicates the type of address being used in the scan
 * request packets.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                Error code
 * MC     Scanning is enabled in the Controller.   Command Disallowed (0x0C)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Scan_Parameters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_scan_params(const sdc_hci_cmd_le_set_scan_params_t * p_params);

/** @brief LE Set Scan Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.11
 *
 * This command is used to start and stop scanning for legacy PDUs (but not extended
 * PDUs, even if the device supports extended advertising). Scanning is used to discover
 * advertising devices nearby.
 *
 * The Filter_Duplicates parameter controls whether the Link Layer should filter out
 * duplicate advertising reports (Filtering_Enabled) to the Host, or if the Link Layer should
 * generate advertising reports for each packet received (Filtering_Disabled). See [Vol 6]
 * Part B, Section 4.4.3.5. If LE_Scan_Enable is set to 0x00 then Filter_Duplicates shall be
 * ignored.
 *
 * If the LE_Scan_Enable parameter is set to 0x01 and scanning is already enabled, any
 * change to the Filter_Duplicates setting shall take effect.
 *
 * Disabling scanning when it is disabled has no effect.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                                   Error code
 * R      LE_Scan_Enable is set to 0x01, the scanning parameters' Own_Address_Type    Invalid HCI
 *        parameter is set to 0x00 or 0x02, and the device does not have a public     Command
 *        address.                                                                    Parameters
 *                                                                                    (0x12)
 * MC     LE_Scan_Enable is set to 0x01, the scanning parameters' Own_Address_Type    Invalid HCI
 *        parameter is set to 0x01 or 0x03, and the random address for the device     Command
 *        has not been initialized using the HCI_LE_Set_Random_Address command.       Parameters
 *                                                                                    (0x12)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Scan_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * Zero or more HCI_LE_Advertising_Report events are generated by the Controller based
 * on legacy advertising packets received and the duplicate filtering. More than one
 * advertising packet may be reported in each HCI_LE_Advertising_Report event. No
 * report shall be issued for extended advertising PDUs.
 *
 * When the Scanning_Filter_Policy is set to 0x02 or 0x03 (see Section 7.8.10)
 * and a directed advertisement was received where the advertiser used a
 * resolvable private address which the Controller is unable to resolve, an
 * HCI_LE_Directed_Advertising_Report event shall be generated instead of an
 * HCI_LE_Advertising_Report event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_scan_enable(const sdc_hci_cmd_le_set_scan_enable_t * p_params);

/** @brief LE Create Connection.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.12
 *
 * This command is used to create a single ACL connection, with the local device in the
 * Central role, to a connectable advertiser.
 *
 * If a connection is created with the local device in the Peripheral role while this command
 * is pending, then this command remains pending.
 *
 * The LE_Scan_Interval and LE_Scan_Window parameters are recommendations from
 * the Host on how long (LE_Scan_Window) and how frequently (LE_Scan_Interval) the
 * Controller should scan. The LE_Scan_Window parameter shall be set to a value smaller
 * or equal to the value set for the LE_Scan_Interval parameter. If both are set to the same
 * value, scanning should run continuously.
 *
 * The Initiator_Filter_Policy is used to determine whether the Filter Accept List is used. If
 * the Filter Accept List is used, then the device to connect to is the first one that sends
 * an advertisement received successfully by the local device and that is on the list. If
 * the Filter Accept List is not used, then the Peer_Address_Type and the Peer_Address
 * parameters specify the address type and address of the advertising device to connect
 * to.
 *
 * Peer_Address_Type parameter indicates the type of address used in the connectable
 * advertisement sent by the peer. The Host shall not set Peer_Address_Type
 * to either 0x02 or 0x03 if both the Host and the Controller support
 * the HCI_LE_Set_Privacy_Mode command. If a Controller that supports the
 * HCI_LE_Set_Privacy_Mode command receives the HCI_LE_Create_Connection
 * command with Peer_Address_Type set to either 0x02 or 0x03, it may use either device
 * privacy mode or network privacy mode for that peer device.
 *
 * Peer_Address parameter indicates the Peer’s Public Device Address, Random (static)
 * Device Address, Non-Resolvable Private Address or Resolvable Private Address
 * depending on the Peer_Address_Type parameter.
 *
 * Own_Address_Type parameter indicates the type of address being used in the
 * connection request packets.
 *
 * The Connection_Interval_Min and Connection_Interval_Max parameters define the
 * minimum and maximum allowed connection interval. The Connection_Interval_Min
 * parameter shall not be greater than the Connection_Interval_Max parameter.
 *
 * The Max_Latency parameter defines the maximum allowed Peripheral latency (see [Vol
 * 6] Part B, Section 4.5.1).
 *
 * The Supervision_Timeout parameter defines the link supervision timeout for the
 * connection. The Supervision_Timeout in milliseconds shall be larger than (1 +
 * Max_Latency) × Connection_Interval_Max × 2, where Connection_Interval_Max is
 * given in milliseconds. (See [Vol 6] Part B, Section 4.5.2).
 *
 * The Min_CE_Length and Max_CE_Length parameters provide the Controller with the
 * expected minimum and maximum length of the connection events. The Controller is not
 * required to use these values.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                                    Error code
 * MC     Another HCI_LE_Create_Connection command is pending in the Controller.       Command
 *                                                                                     Disallowed
 *                                                                                     (0x0C)
 * M      The local device is already connected to the same device address as the      Connection
 *        advertiser (including two different Resolvable Private Addresses that        Already
 *        resolve to the same IRK).                                                    Exists
 *                                                                                     (0x0B)
 * R      Own_Address_Type is set to 0x00 and the device does not have a public        Invalid HCI
 *        address.                                                                     Command
 *                                                                                     Parameters
 *                                                                                     (0x12)
 * MC     Own_Address_Type is set to 0x01 and the random address for the device has    Invalid HCI
 *        not been initialized using the HCI_LE_Set_Random_Address command.            Command
 *                                                                                     Parameters
 *                                                                                     (0x12)
 * R      Own_Address_Type is set to 0x02, Initiator_Filter_Policy is set to 0x00,     Invalid HCI
 *        the Controller's resolving list does not contain a matching entry, and the   Command
 *        device does not have a public address.                                       Parameters
 *                                                                                     (0x12)
 * R      Own_Address_Type is set to 0x02, Initiator_Filter_Policy is set to 0x01,     Invalid HCI
 *        and the device does not have a public address.                               Command
 *                                                                                     Parameters
 *                                                                                     (0x12)
 * MC     Own_Address_Type is set to 0x03, Initiator_Filter_Policy is set to 0x00,     Invalid HCI
 *        the Controller's resolving list does not contain a matching entry, and the   Command
 *        random address for the device has not been initialized using the             Parameters
 *        HCI_LE_Set_Random_Address command.                                           (0x12)
 * MC     Own_Address_Type is set to 0x03, Initiator_Filter_Policy is set to 0x01,     Invalid HCI
 *        and the random address for the device has not been initialized using the     Command
 *        HCI_LE_Set_Random_Address command.                                           Parameters
 *                                                                                     (0x12)
 * R      Max_CE_Length is less than Min_CE_Length                                     Invalid HCI
 *                                                                                     Command
 *                                                                                     Parameters
 *                                                                                     (0x12)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Create_Connection command, the Controller
 * sends the HCI_Command_Status event to the Host. An HCI_LE_Connection_Complete
 * or HCI_LE_Enhanced_Connection_Complete event shall be generated when a
 * connection is created because of this command or the connection creation procedure
 * is cancelled; until one of these events is generated, the command is considered
 * pending. If a connection is created and the Controller supports the LE Channel
 * Selection Algorithm #2 feature, this event shall be immediately followed by an
 * HCI_LE_Channel_Selection_Algorithm event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_create_conn(const sdc_hci_cmd_le_create_conn_t * p_params);

/** @brief LE Create Connection Cancel.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.13
 *
 * This command is used to cancel the HCI_LE_Create_Connection or
 * HCI_LE_Extended_Create_Connection commands.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                            Error code
 * MC     No HCI_LE_Create_Connection or HCI_LE_Extended_Create_Connection     Command Disallowed
 *        command is pending.                                                  (0x0C)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Create_Connection_Cancel command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * If the cancellation was successful then, after the HCI_Command_Complete
 * event for the HCI_LE_Create_Connection_Cancel command, either an
 * HCI_LE_Connection_Complete or an HCI_LE_Enhanced_Connection_Complete event
 * shall be generated. In either case, the event shall be sent with the error code Unknown
 * Connection Identifier (0x02).
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_create_conn_cancel(void);

/** @brief LE Read Filter Accept List Size.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.14
 *
 * This command is used to read the number of Filter Accept List entries (including those
 * already stored there) that the Controller can store at the present time.
 *
 * Note: The number of entries that can be stored is not fixed and the Controller can
 * change it at any time (e.g., because the memory used to store the list can also be used
 * for other purposes).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Filter_Accept_List_Size command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_filter_accept_list_size(sdc_hci_cmd_le_read_filter_accept_list_size_return_t * p_return);

/** @brief LE Clear Filter Accept List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.15
 *
 * This command is used to clear the Filter Accept List stored in the Controller.
 *
 * This command shall not be used when:
 *
 * • any advertising filter policy uses the Filter Accept List and advertising is enabled,
 * • the scanning filter policy uses the Filter Accept List and scanning is enabled, or
 * • the initiator filter policy uses the Filter Accept List and an HCI_LE_Create_Connection
 *   or HCI_LE_Extended_Create_Connection command is pending.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Clear_Filter_Accept_List command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_clear_filter_accept_list(void);

/** @brief LE Add Device To Filter Accept List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.16
 *
 * This command is used to add a single device to the Filter Accept List stored in the
 * Controller.
 *
 * This command shall not be used when:
 *
 * • any advertising filter policy uses the Filter Accept List and advertising is enabled,
 * • the scanning filter policy uses the Filter Accept List and scanning is enabled, or
 * • the initiator filter policy uses the Filter Accept List and an HCI_LE_Create_Connection
 *   or HCI_LE_Extended_Create_Connection command is pending.
 *
 * If the device is already in the Filter Accept List, the Controller should not add the device
 * to the Filter Accept List again and should return success.
 *
 * Address shall be ignored when Address_Type is set to 0xFF.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                           Error code
 * MC     The Controller cannot add a device to the Filter Accept List        Memory Capacity
 *        because there is no space available.                                Exceeded (0x07)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Add_Device_To_Filter_Accept_List command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_add_device_to_filter_accept_list(const sdc_hci_cmd_le_add_device_to_filter_accept_list_t * p_params);

/** @brief LE Remove Device From Filter Accept List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.17
 *
 * This command is used to remove a single device from the Filter Accept List stored in
 * the Controller.
 *
 * This command shall not be used when:
 *
 * • any advertising filter policy uses the Filter Accept List and advertising is enabled,
 * • the scanning filter policy uses the Filter Accept List and scanning is enabled, or
 * • the initiator filter policy uses the Filter Accept List and an HCI_LE_Create_Connection
 *   or HCI_LE_Extended_Create_Connection command is pending.
 *
 * Address shall be ignored when Address_Type is set to 0xFF.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Remove_Device_From_Filter_Accept_List command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_remove_device_from_filter_accept_list(const sdc_hci_cmd_le_remove_device_from_filter_accept_list_t * p_params);

/** @brief LE Connection Update.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.18
 *
 * This command is used to change the ACL connection parameters. This command may
 * be issued on both the Central and Peripheral.
 *
 * The Connection_Interval_Min and Connection_Interval_Max parameters are
 * used to define the minimum and maximum allowed connection interval.
 * The Connection_Interval_Min parameter shall not be greater than the
 * Connection_Interval_Max parameter.
 *
 * The Max_Latency parameter shall define the maximum allowed Peripheral latency.
 *
 * The Supervision_Timeout parameter shall define the link supervision timeout for the LE
 * link. The Supervision_Timeout in milliseconds shall be larger than (1 + Max_Latency)
 * × Subrate_Factor × Connection_Interval_Max × 2, where Connection_Interval_Max is
 * given in milliseconds and Subrate_Factor is the current subrate factor of the connection.
 *
 * The Min_CE_Length and Max_CE_Length are information parameters providing the
 * Controller with a hint about the expected minimum and maximum length of the
 * connection events.
 *
 * The actual parameter values selected by the Link Layer may be different from the
 * parameter values provided by the Host through this command.
 *
 * If this command completes successfully and the connection interval has changed,
 * then the subrating factor shall be set to 1 and the continuation number to 0. In this
 * case, Max_Latency must be interpreted in underlying connection events. Otherwise the
 * subrating factor and continuation number shall be unchanged and Max_Latency must
 * be interpreted in subrated events.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 * <pre>
 * Type   Condition                                                            Error code
 * MC     One or more CS procedures are enabled.                               Command Disallowed
 *                                                                             (0x0C)
 * M      The Controller is the Peripheral and the local Controller does not   Unsupported Feature
 *        support the Connection Parameters Request procedure (see [Vol 6]     or Parameter Value
 *        Part B, Section 5.1.7).                                              (0x11)
 * M      The Controller is the Peripheral and the local Controller supports   Unsupported Remote
 *        the Connection Parameters Request procedure but the peer             Feature (0x1A)
 *        Controller does not.
 * R      Max_CE_Length is less than Min_CE_Length                             Invalid HCI Command
 *                                                                             Parameters (0x12)
 * MC     A Connection Rate Request procedure or a Connection Rate Update      Command Disallowed
 *        procedure is in progress.                                            (0x0C)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Connection_Update command,
 * the Controller sends the HCI_Command_Status event to the Host. The
 * HCI_LE_Connection_Update_Complete event shall be generated after the connection
 * parameters have been applied by the Controller or if the command subsequently fails.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_conn_update(const sdc_hci_cmd_le_conn_update_t * p_params);

/** @brief LE Set Host Channel Classification.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.19
 *
 * This command allows the Host to specify a channel classification for the data,
 * secondary advertising, periodic, and isochronous physical channels based on its
 * “local information”. This classification persists until overwritten with a subsequent
 * HCI_LE_Set_Host_Channel_Classification command or until the Controller is reset
 * using the HCI_Reset command (see [Vol 6] Part B, Section 4.5.8.1).
 *
 * If this command is used, the Host should send it within 10 seconds of knowing that the
 * channel classification has changed. The interval between two successive commands
 * sent shall be at least one second.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Host_Channel_Classification command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_host_channel_classification(const sdc_hci_cmd_le_set_host_channel_classification_t * p_params);

/** @brief LE Read Channel Map.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.20
 *
 * This command returns the current Channel_Map for the specified Connection_Handle.
 * The returned value indicates the state of the Channel_Map specified by
 * the last transmitted or received Channel_Map (in a CONNECT_IND or
 * LL_CHANNEL_MAP_IND message) for the specified Connection_Handle, regardless
 * of whether the Central has received an acknowledgment. If the connection handle does
 * not identify an ACL connection, the Controller shall reject the command and should
 * return the error code Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Channel_Map command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_channel_map(const sdc_hci_cmd_le_read_channel_map_t * p_params,
                                        sdc_hci_cmd_le_read_channel_map_return_t * p_return);

/** @brief LE Read Remote Features Page 0.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.21
 *
 * This command requests, from the remote device identified by the Connection_Handle,
 * page 0 of the features used on the connection and the features supported by the remote
 * device. For details see [Vol 6] Part B, Section 4.6.
 *
 * This command may be issued on both the Central and Peripheral.
 *
 * If a connection already exists between the two devices and page 0 of the features have
 * already been fetched on that connection, the Controller may use a cached copy of page
 * 0 of the features.
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Read_Remote_Features_Page_0 command,
 * the Controller shall send the HCI_Command_Status event to the Host. When the
 * Controller has completed the procedure to determine the remote features or has
 * determined that it will be using a cached copy, the Controller shall send an
 * HCI_LE_Read_Remote_Features_Page_0_Complete event to the Host.
 *
 * The HCI_LE_Read_Remote_Features_Page_0_Complete event contains the status of
 * this command and the parameter describing page 0 of the features used on the
 * connection and the features supported by the remote device.
 *
 * This command was formerly called "LE Read Remote Features".
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_remote_features(const sdc_hci_cmd_le_read_remote_features_t * p_params);

/** @brief LE Encrypt.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.22
 *
 * This command is used to request the Controller to encrypt the Plaintext_Data in the
 * command using the Key given in the command and returns the Encrypted_Data to
 * the Host. The AES-128 bit block cypher is defined in NIST Publication FIPS-197 (http://
 * csrc.nist.gov/publications/fips/fips197/fips-197.pdf).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Encrypt command has completed, an HCI_Command_Complete
 * event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_encrypt(const sdc_hci_cmd_le_encrypt_t * p_params,
                               sdc_hci_cmd_le_encrypt_return_t * p_return);

/** @brief LE Rand.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.23
 *
 * This command is used to request the Controller to generate 8 octets of random data to
 * be sent to the Host. The Random_Number shall be generated according to [Vol 2] Part
 * H, Section 2.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Rand command has completed, an HCI_Command_Complete event
 * shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_rand(sdc_hci_cmd_le_rand_return_t * p_return);

/** @brief LE Enable Encryption.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.24
 *
 * This command is used to authenticate the given encryption key associated with the
 * remote device specified by the Connection_Handle, and once authenticated will encrypt
 * the connection. The parameters are as defined in [Vol 3] Part H, Section 2.4.4.
 *
 * If the connection is already encrypted then the Controller shall pause connection
 * encryption before attempting to authenticate the given encryption key, and then re-
 * encrypt the connection. While encryption is paused no user data shall be transmitted.
 *
 * If the Connection_Handle parameter identifies an ACL with an associated CIS that has
 * been created, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * On an authentication failure, the connection shall be automatically disconnected by the
 * Link Layer. If this command succeeds, then the connection shall be encrypted.
 *
 * This command shall only be used when the local device’s role is Central.
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Enable_Encryption command it shall send
 * the HCI_Command_Status event to the Host. If the connection is not encrypted
 * when this command is issued, an HCI_Encryption_Change event shall occur when
 * encryption has been started for the connection. If the connection is encrypted when
 * this command is issued, an HCI_Encryption_Key_Refresh_Complete event shall occur
 * when encryption has been resumed.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_enable_encryption(const sdc_hci_cmd_le_enable_encryption_t * p_params);

/** @brief LE Long Term Key Request Reply.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.25
 *
 * This command is used to reply to an HCI_LE_Long_Term_Key_Request event from
 * the Controller, and specifies the Long_Term_Key parameter that shall be used for
 * this Connection_Handle. The Long_Term_Key is used as defined in [Vol 6] Part B,
 * Section 5.1.3.
 *
 * This command shall only be used when the local device’s role is Peripheral.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Long_Term_Key_Request_Reply command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_long_term_key_request_reply(const sdc_hci_cmd_le_long_term_key_request_reply_t * p_params,
                                                   sdc_hci_cmd_le_long_term_key_request_reply_return_t * p_return);

/** @brief LE Long Term Key Request Negative Reply.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.26
 *
 * This command is used to reply to an HCI_LE_Long_Term_Key_Request event from the
 * Controller if the Host cannot provide a Long Term Key for this Connection_Handle.
 *
 * This command shall only be used when the local device’s role is Peripheral.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Long_Term_Key_Request_Negative_Reply command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_long_term_key_request_negative_reply(const sdc_hci_cmd_le_long_term_key_request_negative_reply_t * p_params,
                                                            sdc_hci_cmd_le_long_term_key_request_negative_reply_return_t * p_return);

/** @brief LE Read Supported States.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.27
 *
 * This command reads the states, roles, and combinations of states and roles that the
 * Link Layer supports. See [Vol 6] Part B, Section 1.1.1.
 *
 * The LE_States parameter is a bit field. If a bit is set to 1 then this state, role, or
 * combination is supported by the Controller; each combination consists of the states
 * indicated by 'X' in the associated row of the table and omits those states with an empty
 * cell in that row. Multiple bits in LE_States may be set to 1 to indicate support for multiple
 * combinations.
 *
 * Note: This command only provides information about the supported states and roles
 * that can be used with legacy advertising. It does not provide information about those
 * that can only be used with the extended advertising commands (see Section 3.1.1).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Supported_States command has completed, an
 * HCI_Command_Complete event will be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_supported_states(sdc_hci_cmd_le_read_supported_states_return_t * p_return);

/** @brief LE Receiver Test [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.28
 *
 * This command is used to start a test where the IUT receives test reference packets at a
 * fixed interval. The tester generates the test reference packets.
 *
 * The RX_Channel parameter specifies the RF channel to be used by
 * the receiver.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Receiver_Test command has completed, an HCI_Command_-
 * Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_receiver_test_v1(const sdc_hci_cmd_le_receiver_test_v1_t * p_params);

/** @brief LE Transmitter Test [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.29
 *
 * This command is used to start a test where the IUT generates test reference packets
 * at a fixed interval. The Controller shall transmit at the power level indicated by the
 * TX_Power_Level parameter.
 *
 * The TX_Channel specifies the RF channel to be used by
 * the transmitter.
 *
 * The Test_Data_Length and Packet_Payload parameters specify the length and
 * contents of the Payload of the test reference packets. An LE Controller supporting
 * the HCI_LE_Transmitter_Test command shall support Packet_Payload values 0x00,
 * 0x01 and 0x02. An LE Controller supporting the LE Coded PHY shall also
 * support Packet_Payload value 0x04. An LE Controller may support other values of
 * Packet_Payload.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Transmitter_Test command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_transmitter_test_v1(const sdc_hci_cmd_le_transmitter_test_v1_t * p_params);

/** @brief LE Test End.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.30
 *
 * This command is used to stop any test which is in progress. The Num_Packets for a
 * transmitter test shall be reported as 0x0000. The Num_Packets is an unsigned number
 * and contains the number of received packets.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Test_End command has completed, an HCI_Command_Complete
 * event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_test_end(sdc_hci_cmd_le_test_end_return_t * p_return);

/** @brief LE Set Data Length.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.33
 *
 * This command allows the Host to suggest the maximum transmission payload size and
 * maximum packet transmission time (connMaxTxOctets and connMaxTxTime - see [Vol
 * 6] Part B, Section 4.5.10) to be used for LL Data PDUs on a given connection. The
 * Controller may use smaller or larger values based on local information.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Data_Length command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * If the command causes the maximum transmission packet size or maximum packet
 * transmission time to change, an HCI_LE_Data_Length_Change event shall be
 * generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_data_length(const sdc_hci_cmd_le_set_data_length_t * p_params,
                                       sdc_hci_cmd_le_set_data_length_return_t * p_return);

/** @brief LE Read Suggested Default Data Length.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.34
 *
 * This command allows the Host to read the Host's suggested values
 * (Suggested_Max_TX_Octets and Suggested_Max_TX_Time) for the Controller's
 * maximum transmitted number of payload octets and maximum packet transmission time
 * for packets containing LL Data PDUs to be used for new connections (see [Vol 6] Part
 * B, Section 4.5.10).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Suggested_Default_Data_Length command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_suggested_default_data_length(sdc_hci_cmd_le_read_suggested_default_data_length_return_t * p_return);

/** @brief LE Write Suggested Default Data Length.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.35
 *
 * This command allows the Host to specify its suggested values for the Controller's
 * maximum transmission number of payload octets and maximum packet transmission
 * time for packets containing LL Data PDUs to be used for new connections.
 * The Controller may use smaller or larger values for connInitialMaxTxOctets and
 * connInitialMaxTxTime based on local information. (See [Vol 6] Part B, Section 4.5.10).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Write_Suggested_Default_Data_Length command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_write_suggested_default_data_length(const sdc_hci_cmd_le_write_suggested_default_data_length_t * p_params);

/** @brief LE Add Device To Resolving List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.38
 *
 * This command is used to add one device to the resolving list used to generate and
 * resolve Resolvable Private Addresses in the Controller.
 *
 * This command shall not be used when address resolution is enabled in the Controller
 * and:
 *
 * • Advertising (other than periodic advertising) is enabled,
 * • Scanning is enabled, or
 * • an HCI_LE_Create_Connection, HCI_LE_Extended_Create_Connection, or
 *   HCI_LE_Periodic_Advertising_Create_Sync command is pending.
 *
 * This command may be used at any time when address resolution is disabled in the
 * Controller.
 *
 * The added device shall be set to Network Privacy mode.
 *
 * If Peer_Identity_Address_Type is 0x01 and Peer_Identity_Address is not a static
 * address, then the Controller should return the error code Invalid HCI Command
 * Parameters (0x12).
 *
 * When a Controller cannot add a device to the list because there is no space available, it
 * shall return the error code Memory Capacity Exceeded (0x07).
 *
 * If an entry already exists in the resolving list with the same four parameter values, the
 * Controller shall either reject the command or not add the device to the resolving list
 * again and return success. If the command is rejected then the error code Invalid HCI
 * Command Parameters (0x12) should be used.
 *
 * If there is an existing entry in the resolving list with the same Peer_Identity_Address
 * and Peer_Identity_Address_Type, or with the same non-zero Peer_IRK, the Controller
 * should return the error code Invalid HCI Command Parameters (0x12).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Add_Device_To_Resolving_List command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_add_device_to_resolving_list(const sdc_hci_cmd_le_add_device_to_resolving_list_t * p_params);

/** @brief LE Remove Device From Resolving List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.39
 *
 * This command is used to remove one device from the resolving list used to resolve
 * Resolvable Private Addresses in the Controller.
 *
 * This command shall not be used when address resolution is enabled in the Controller
 * and:
 *
 * • Advertising (other than periodic advertising) is enabled,
 * • Scanning is enabled, or
 * • an HCI_LE_Create_Connection, HCI_LE_Extended_Create_Connection, or
 *   HCI_LE_Periodic_Advertising_Create_Sync command is pending.
 *
 * This command may be used at any time when address resolution is disabled in the
 * Controller.
 *
 * When a Controller cannot remove a device from the resolving list because it is not
 * found, it shall return the error code Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Remove_Device_From_Resolving_List command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_remove_device_from_resolving_list(const sdc_hci_cmd_le_remove_device_from_resolving_list_t * p_params);

/** @brief LE Clear Resolving List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.40
 *
 * This command is used to remove all devices from the resolving list used to resolve
 * Resolvable Private Addresses in the Controller.
 *
 * This command shall not be used when address resolution is enabled in the Controller
 * and:
 *
 * • Advertising (other than periodic advertising) is enabled,
 * • Scanning is enabled, or
 * • an HCI_LE_Create_Connection, HCI_LE_Extended_Create_Connection, or
 *   HCI_LE_Periodic_Advertising_Create_Sync command is pending.
 *
 * This command may be used at any time when address resolution is disabled in the
 * Controller.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Clear_Resolving_List command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_clear_resolving_list(void);

/** @brief LE Read Resolving List Size.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.41
 *
 * This command is used to read the total number of entries in the resolving list that can
 * be stored in the Controller.
 *
 * Note: The number of entries that can be stored is not fixed and the Controller can
 * change it at any time (e.g. because the memory used to store the list can also be used
 * for other purposes).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Resolving_List_Size command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_resolving_list_size(sdc_hci_cmd_le_read_resolving_list_size_return_t * p_return);

/** @brief LE Set Address Resolution Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.44
 *
 * This command is used to enable resolution of Resolvable Private Addresses in the
 * Controller. This causes the Controller to use the resolving list whenever the Controller
 * receives a local or peer Resolvable Private Address.
 *
 * This command shall not be used when:
 *
 * • Advertising (other than periodic advertising) is enabled,
 * • Scanning is enabled, or
 * • an HCI_LE_Create_Connection, HCI_LE_Extended_Create_Connection, or
 *   HCI_LE_Periodic_Advertising_Create_Sync command is pending.
 *
 * Enabling address resolution when it is already enabled, or disabling it when it is already
 * disabled, has no effect.
 *
 * The requirements in [Vol 6] Part B, Section 6 related to the generation of Resolvable
 * Private Addresses and the privacy of the device are independent of this command.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Address_Resolution_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_address_resolution_enable(const sdc_hci_cmd_le_set_address_resolution_enable_t * p_params);

/** @brief LE Set Resolvable Private Address Timeout [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.45
 *
 * Version [v1] of this command sets the length of time the Controller uses a Resolvable
 * Private Address before a new Resolvable Private Address is generated and starts being
 * used.
 *
 * Version [v2] of this command sets the range of time the Controller uses a Resolvable
 * Private Address before a new Resolvable Private Address is generated and starts being
 * used.
 *
 * This timeout applies to all resolvable private addresses generated by the Controller.
 *
 * The RPA_Timeout parameter specifies the time after which a new Resolvable Private
 * Address shall start being used.
 *
 * The RPA_Timeout_Min parameter specifies the minimum time after which a new
 * Resolvable Private Address shall start being used.
 *
 * The RPA_Timeout_Max parameter specifies the maximum time after which a new
 * Resolvable Private Address shall start being used.
 *
 * When the Controller supports the HCI_LE_Set_Resolvable_Private_Address_Timeout
 * [v2] command and needs to set a new timeout (e.g., when the RPA is set for the
 * first time, or when the current timeout expires), the new timeout shall be a random
 * value between RPA_Timeout_Min and RPA_Timeout_Max generated so as to meet the
 * requirements for random number generation defined in [Vol 2] Part H, Section 2.
 *
 * If RPA_Timeout_Min is greater than RPA_Timeout_Max, then the Controller shall return
 * the error code Invalid HCI Command Parameters (0x12).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Resolvable_Private_Address_Timeout command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_resolvable_private_address_timeout(const sdc_hci_cmd_le_set_resolvable_private_address_timeout_t * p_params);

/** @brief LE Read Maximum Data Length.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.46
 *
 * This command allows the Host to read the Controller’s maximum supported
 * payload octets and packet duration times for transmission and reception
 * (Supported_Max_TX_Octets, Supported_Max_TX_Time, Supported_Max_RX_Octets,
 * and Supported_Max_RX_Time, see [Vol 6] Part B, Section 4.5.10).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Maximum_Data_Length command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_max_data_length(sdc_hci_cmd_le_read_max_data_length_return_t * p_return);

/** @brief LE Read PHY.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.47
 *
 * This command is used to read the current transmitter PHY and receiver PHY on the
 * connection identified by the Connection_Handle.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_PHY command has completed, an HCI_Command_Complete
 * event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_phy(const sdc_hci_cmd_le_read_phy_t * p_params,
                                sdc_hci_cmd_le_read_phy_return_t * p_return);

/** @brief LE Set Default PHY.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.48
 *
 * This command allows the Host to specify its preferred values for the transmitter PHY
 * and receiver PHY to be used for all subsequent connections over the LE transport.
 *
 * The All_PHYs parameter is a bit field that allows the Host to specify, for each direction,
 * whether it has no preference among the PHYs that the Controller supports in a given
 * direction or whether it has specified particular PHYs that it prefers in the TX_PHYs or
 * RX_PHYs parameter.
 *
 * The TX_PHYs parameter is a bit field that indicates the transmitter PHYs that the Host
 * prefers the Controller to use. If the All_PHYs parameter specifies that the Host has no
 * preference, the TX_PHYs parameter shall be ignored; otherwise at least one bit shall be
 * set to 1.
 *
 * The RX_PHYs parameter is a bit field that indicates the receiver PHYs that the Host
 * prefers the Controller to use. If the All_PHYs parameter specifies that the Host has no
 * preference, the RX_PHYs parameter shall be ignored; otherwise at least one bit shall
 * be set to 1.
 *
 * If the Host sets, in the TX_PHYs or RX_PHYs parameter, a bit for a PHY that the
 * Controller does not support, including a bit that is reserved for future use, the Controller
 * shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Controller does not support asymmetric connections (see [Vol 6] Part B,
 * Section 4.6.9.1) and the Host sets All_PHYs to 0x00 and TX_PHYs to a different
 * value than RX_PHYs, the Controller shall return the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Default_PHY command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_default_phy(const sdc_hci_cmd_le_set_default_phy_t * p_params);

/** @brief LE Set PHY.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.49
 *
 * This command is used to set the PHY preferences for the connection identified by
 * the Connection_Handle. The Controller might not be able to make the change (e.g.
 * because the peer does not support the requested PHY) or may decide that the current
 * choice of PHY is preferable (e.g., because it could not schedule other activities if the
 * requested PHY was used or because it requires a PHY that supports Constant Tone
 * Extensions).
 *
 * The All_PHYs parameter is a bit field that allows the Host to specify, for each direction,
 * whether it has no preference among the PHYs that the Controller supports in a given
 * direction or whether it has specified particular PHYs that it prefers in the TX_PHYs or
 * RX_PHYs parameter.
 *
 * The TX_PHYs parameter is a bit field that indicates the transmitter PHYs that the Host
 * prefers the Controller to use. If the All_PHYs parameter specifies that the Host has no
 * preference, the TX_PHYs parameter shall be ignored; otherwise at least one bit shall be
 * set to 1.
 *
 * The RX_PHYs parameter is a bit field that indicates the receiver PHYs that the Host
 * prefers the Controller to use. If the All_PHYs parameter specifies that the Host has no
 * preference, the RX_PHYs parameter shall be ignored; otherwise at least one bit shall
 * be set to 1.
 *
 * The Controller shall request a change unless it determines that this is unnecessary or
 * that the current PHY in both directions is preferable, in which case it may, but need not,
 * request a change.
 *
 * The PHY preferences provided by the HCI_LE_Set_PHY command override those
 * provided via the HCI_LE_Set_Default_PHY command (Section 7.8.48) or any
 * preferences previously set using the HCI_LE_Set_PHY command on the same
 * connection.
 * The PHY_Options parameter is a bit field that allows the Host to specify options for
 * PHYs. The default value for a new connection shall be all zero bits. The Controller may
 * override any preferred coding for transmitting on the LE Coded PHY.
 *
 * The Host may specify a preferred coding even if it prefers not to use the LE Coded
 * transmitter PHY since the Controller may override the PHY preference.
 *
 * If the Host sets, in the TX_PHYs or RX_PHYs parameter, a bit for a PHY that the
 * Controller does not support, including a bit that is reserved for future use, the Controller
 * shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Controller does not support asymmetric connections (see [Vol 6] Part B,
 * Section 4.6.9.1) and the Host sets All_PHYs to 0x00 and TX_PHYs to a different
 * value than RX_PHYs, the Controller shall return the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Set_PHY command, the
 * Controller shall send the HCI_Command_Status event to the Host. The
 * HCI_LE_PHY_Update_Complete event shall be generated either when one or both
 * PHY changes or when the Controller determines that neither PHY will change
 * immediately.
 *
 * Note: If the peer negotiation resulted in no change to either PHY, this is not an error and
 * the HCI_LE_PHY_Update_Complete event will contain a status indicating success.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_phy(const sdc_hci_cmd_le_set_phy_t * p_params);

/** @brief LE Receiver Test [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.28
 *
 * This command is used to start a test where the IUT receives test reference packets at a
 * fixed interval. The tester generates the test reference packets.
 *
 * The RX_Channel and PHY parameters specify the RF channel and PHY to be used by
 * the receiver. If the Host sets the PHY parameter to a PHY that the Controller does not
 * support, including a value that is reserved for future use, the Controller shall return the
 * error code Unsupported Feature or Parameter Value (0x11).
 *
 * The Modulation_Index parameter specifies whether or not the Controller should assume
 * the receiver has a stable modulation index.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Receiver_Test command has completed, an HCI_Command_-
 * Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_receiver_test_v2(const sdc_hci_cmd_le_receiver_test_v2_t * p_params);

/** @brief LE Transmitter Test [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.29
 *
 * This command is used to start a test where the IUT generates test reference packets
 * at a fixed interval. The Controller shall transmit at the power level indicated by the
 * TX_Power_Level parameter.
 *
 * The TX_Channel and PHY parameters specify the RF channel and PHY to be used by
 * the transmitter. If the Host sets the PHY parameter to a PHY that the Controller does
 * not support, including a value that is reserved for future use, the Controller shall return
 * the error code Unsupported Feature or Parameter Value (0x11).
 *
 * The Test_Data_Length and Packet_Payload parameters specify the length and
 * contents of the Payload of the test reference packets. An LE Controller supporting
 * the HCI_LE_Transmitter_Test command shall support Packet_Payload values 0x00,
 * 0x01 and 0x02. An LE Controller supporting the LE Coded PHY shall also
 * support Packet_Payload value 0x04. An LE Controller may support other values of
 * Packet_Payload.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Transmitter_Test command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_transmitter_test_v2(const sdc_hci_cmd_le_transmitter_test_v2_t * p_params);

/** @brief LE Set Advertising Set Random Address.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.52
 *
 * This command is used by the Host to set the random device address specified by the
 * Random_Address parameter. This address is used in the Controller (see [Vol 6] Part B,
 * Section 1.3.2 ) for the advertiser's address contained in the advertising PDUs for the
 * advertising set specified by the Advertising_Handle parameter.
 *
 * This command may be issued at any time after an advertising set
 * identified by the Advertising_Handle parameter has been created using the
 * HCI_LE_Set_Extended_Advertising_Parameters command (see Section 7.8.53).
 * However, if the Host issues this command while the advertising set identified by the
 * Advertising_Handle parameter is using connectable advertising and is enabled, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If this command is used to change the address, the new random
 * address shall take effect for advertising no later than the next successful
 * HCI_LE_Set_Extended_Advertising_Enable command and for periodic advertising no
 * later than the next successful HCI_LE_Periodic_Advertising_Enable command.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Advertising_Set_Random_Address command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_adv_set_random_address(const sdc_hci_cmd_le_set_adv_set_random_address_t * p_params);

/** @brief LE Set Extended Advertising Parameters [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.53
 *
 * This command is used by the Host to set the advertising parameters.
 *
 * The Advertising_Handle parameter identifies the advertising set whose parameters are
 * being configured.
 *
 * The Advertising_Event_Properties parameter describes the type of advertising event
 * that is being configured and its basic properties. The type shall be one supported by the
 * Controller. In particular, the following restrictions apply to this parameter:
 *
 * • If legacy advertising PDU types are being used, then the parameter value shall be
 *   one of those specified in Table 7.3. If the advertising set already contains data, the
 *   type shall be one that supports advertising data and the amount of data shall not
 *   exceed 31 octets.
 *
 *  Event Type                    PDU Type               Advertising Event     Advertising Data
 *                                                       Properties
 *  Connectable and scannable     ADV_IND                0b00000000_00010011   Supported
 *  undirected
 *  Connectable directed          ADV_DIRECT_IND         0b00000000_00010101   Not allowed
 *  (low duty cycle)
 *  Connectable directed          ADV_DIRECT_IND         0b00000000_00011101   Not allowed
 *  (high duty cycle)
 *  Scannable undirected          ADV_SCAN_IND           0b00000000_00010010   Supported
 *  Non-connectable and non-      ADV_NON-               0b00000000_00010000   Supported
 *  scannable undirected          CONN_IND
 *
 * Table 7.3: Advertising_Event_Properties values for legacy PDUs
 *
 * • If extended advertising PDU types are being used (bit 4 = 0), then the advertisement
 *   shall not be both connectable and scannable (bits 0 and 1 must not both be set to 1)
 *   and high duty cycle directed connectable advertising (≤ 3.75 ms advertising interval)
 *   shall not be used (bit 3 = 0).
 *
 * If the Advertising_Event_Properties parameter does not describe an event type
 * supported by the Controller, contains an invalid bit combination, or specifies a type
 * that does not support advertising data when the advertising set already contains some,
 * the Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * The parameters beginning with “Secondary” are only valid when extended advertising
 * PDU types are being used (bit 4 = 0).
 *
 * The Own_Address_Type parameter shall be ignored for undirected anonymous
 * advertising (bit 2 = 0 and bit 5 = 1).
 * If Directed advertising is selected, the Peer_Address_Type and Peer_Address shall be
 * valid and the Advertising_Filter_Policy parameter shall be ignored.
 *
 * The Primary_Advertising_Interval_Min parameter shall be less than or equal to the
 * Primary_Advertising_Interval_Max parameter. The Primary_Advertising_Interval_Min
 * and Primary_Advertising_Interval_Max parameters should not be the same value so
 * that the Controller can choose the best advertising interval given other activities.
 *
 * For high duty cycle connectable directed advertising event type (ADV_DIRECT_IND),
 * the Primary_Advertising_Interval_Min and Primary_Advertising_Interval_Max
 * parameters are not used and shall be ignored.
 *
 * If the primary advertising interval range provided by the Host
 * (Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max) does not
 * overlap with the advertising interval range supported by the Controller, then the
 * Controller shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * The Primary_Advertising_Channel_Map is a bit field that indicates the advertising
 * channel indices that shall be used when transmitting advertising packets. At least one
 * channel bit shall be set in the Primary_Advertising_Channel_Map parameter.
 *
 * The Own_Address_Type parameter specifies the type of address being used in
 * the advertising packets. For random addresses, the address is specified by the
 * HCI_LE_Set_Advertising_Set_Random_Address command.
 *
 * If Own_Address_Type equals 0x02 or 0x03, the Peer_Address parameter contains
 * the peer’s Identity Address and the Peer_Address_Type parameter contains the
 * peer’s Identity Type (i.e., 0x00 or 0x01). These parameters are used to locate the
 * corresponding local IRK in the resolving list; this IRK is used to generate their own
 * address used in the advertisement.
 *
 * The Advertising_TX_Power parameter indicates the maximum power level at which the
 * advertising packets are to be transmitted on the advertising physical channels. The
 * Controller shall choose a power level lower than or equal to the one specified by the
 * Host.
 *
 * The Primary_Advertising_PHY parameter indicates the PHY on which the advertising
 * packets are transmitted on the primary advertising physical channel. If legacy
 * advertising PDUs are being used, the Primary_Advertising_PHY shall indicate the LE
 * 1M PHY. The Secondary_Advertising_PHY parameter indicates the PHY on which the
 * advertising packets are be transmitted on the secondary advertising physical channel.
 * If the Host specifies a PHY that is not supported by the Controller, including a value
 * that is reserved for future use, it should return the error code Unsupported Feature or
 * Parameter Value (0x11). If Constant Tone Extensions are enabled for the advertising set
 * and Secondary_Advertising_PHY specifies a PHY that does not allow Constant Tone
 * Extensions, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Primary_Advertising_PHY indicates the LE Coded PHY, then the
 * Primary_Advertising_PHY_Options shall indicate the Host's preference or requirement
 * concerning coding scheme. Otherwise, Primary_Advertising_PHY_Options shall
 * be ignored. If the Secondary_Advertising_PHY indicates the LE Coded PHY,
 * then the Secondary_Advertising_PHY_Options shall indicate the Host's preference
 * or requirement concerning coding scheme (including for periodic advertising).
 * Otherwise, Secondary_Advertising_PHY_Options shall be ignored. If the Host
 * specifies that it requires a specific coding (i.e., value 0x03 or 0x04) in the
 * Primary_Advertising_PHY_Options or Secondary_Advertising_PHY_Options and the
 * Controller supports the LE Feature (Advertising Coding Selection) but is currently
 * unable to provide all the required settings, then the Controller shall return the error
 * code Command Disallowed (0x0C).
 *
 * The Secondary_Advertising_Max_Skip parameter is the maximum number of
 * advertising events that can be skipped before the AUX_ADV_IND can be sent.
 *
 * The Advertising_SID parameter specifies the value to be transmitted in the Advertising
 * SID subfield of the ADI field of the Extended Header of those advertising physical
 * channel PDUs that have an ADI field. If the advertising set only uses PDUs that do not
 * contain an ADI field, Advertising_SID shall be ignored.
 *
 * The Scan_Request_Notification_Enable parameter indicates whether the Controller
 * shall send notifications upon the receipt of a scan request PDU that is in response
 * to an advertisement from the specified advertising set that contains its device address
 * and is from a scanner that is allowed by the advertising filter policy.
 *
 * The Controller shall set the Selected_TX_Power parameter to the transmit power that
 * it will use for transmitting the advertising packets for the specified advertising set. The
 * Controller shall only change this value if requested by the Host. If the radiated power
 * level will vary between packets (e.g., because of frequency-dependent properties of the
 * transmitter) then the value should be the best estimate of the maximum power used.
 *
 * If the Host issues this command when advertising is enabled for the specified
 * advertising set, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host issues this command when periodic advertising is enabled for the specified
 * advertising set and connectable, scannable, legacy, or anonymous advertising is
 * specified, the Controller shall return the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If periodic advertising is enabled for the advertising set and the
 * Secondary_Advertising_PHY parameter does not specify the PHY currently being
 * used for the periodic advertising, the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * If the Advertising_Handle does not identify an existing advertising set and the Controller
 * is unable to support a new advertising set at present, the Controller shall return the error
 * code Memory Capacity Exceeded (0x07).
 *
 * If the advertising set already contains advertising data or scan response data, extended
 * advertising is being used, and the length of the data is greater than the maximum that
 * the Controller can transmit within the longest possible auxiliary advertising segment
 * consistent with the parameters, the Controller shall return the error code Packet Too
 * Long (0x45). If advertising on the LE Coded PHY, the S=8 coding shall be assumed
 * unless the current advertising parameters require the use of S=2 for an advertising
 * physical channel, in which case the S=2 coding shall be assumed for that advertising
 * physical channel.
 *
 * If the Controller does not support the LE Feature (Advertising Coding
 * Selection) and the Host does not set both Primary_Advertising_PHY_Options and
 * Secondary_Advertising_PHY_Options to zero, the Controller shall return the error code
 * Unsupported Feature or Parameter Value (0x11).
 *
 * Missing parameters:
 *
 * When a version of this command is issued that does not include all the parameters, the
 * following values shall be used for any missing parameters:
 *
 *  Parameter                                                                   Value
 *  Primary_Advertising_PHY_Options                                             0x00
 *  Secondary_Advertising_PHY_Options                                           0x00
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Extended_Advertising_Parameters command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_ext_adv_params(const sdc_hci_cmd_le_set_ext_adv_params_t * p_params,
                                          sdc_hci_cmd_le_set_ext_adv_params_return_t * p_return);

/** @brief LE Set Extended Advertising Data.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.54
 *
 * This command is used to set the data used in advertising PDUs that have
 * a data field. This command may be issued at any time after an advertising
 * set identified by the Advertising_Handle parameter has been created using
 * the HCI_LE_Set_Extended_Advertising_Parameters command (see Section 7.8.53),
 * regardless of whether advertising in that set is enabled or disabled.
 *
 * If advertising is currently enabled for the specified advertising set, the Controller shall
 * use the new data in subsequent extended advertising events for this advertising set.
 * If an extended advertising event is in progress when this command is issued, the
 * Controller may use the old or new data for that event.
 *
 * If advertising is currently disabled for the specified advertising set, the data shall be kept
 * by the Controller and used once advertising is enabled for that set. The data shall be
 * discarded when the advertising set is removed.
 *
 * Only the significant part of the advertising data should be transmitted in the advertising
 * packets as defined in [Vol 3] Part C, Section 11.
 *
 * The Host may set the advertising data in one or more operations using the Operation
 * parameter in the command. If the combined length of the data exceeds the capacity of
 * the advertising set identified by the Advertising_Handle parameter (see Section 7.8.57
 * LE Read Maximum Advertising Data Length command) or the amount of memory
 * currently available, all the data shall be discarded and the Controller shall return the
 * error code Memory Capacity Exceeded (0x07).
 *
 * If the advertising set uses extended advertising and the combined length of the data is
 * greater than the maximum that the Controller can transmit within the longest possible
 * auxiliary advertising segment consistent with the current parameters of the advertising
 * set (using the current advertising interval if advertising is enabled), all the data shall
 * be discarded and the Controller shall return the error code Packet Too Long (0x45). If
 * advertising on the LE Coded PHY, the S=8 coding shall be assumed unless the current
 * advertising parameters require the use of S=2 for an advertising physical channel, in
 * which case the S=2 coding shall be assumed for that advertising physical channel.
 * If Operation indicates the start of new data (values 0x01 or 0x03), then any existing
 * partial or complete advertising data shall be discarded.
 *
 * If the advertising data is discarded by the command or the combined length of the data
 * after the command completes is zero, the advertising set will have no advertising data.
 *
 * If Operation is 0x04, the behavior is the same as if the current advertising data had
 * been sent again; this can be used to cause the Advertising DID value to be updated
 * (see [Vol 6] Part B, Section 4.4.2.11).
 *
 * The Fragment_Preference parameter provides a hint to the Controller as to whether
 * advertising data should be fragmented.
 *
 * If the advertising set specifies a type that does not support advertising data, the
 * Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the advertising set uses legacy advertising PDUs that support advertising data and
 * either Operation is not 0x03 or the Advertising_Data_Length parameter exceeds 31
 * octets, the Controller shall return the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If Operation is 0x04 and:
 *
 * • advertising is currently disabled for the advertising set;
 * • the advertising set contains no data;
 * • the advertising set uses legacy PDUs; or
 * • Advertising_Data_Length is not zero;
 *
 * then the Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If Operation is not 0x03 or 0x04 and Advertising_Data_Length is zero, the Controller
 * shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If advertising is currently enabled for the specified advertising set and Operation does
 * not have the value 0x03 or 0x04, the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * then the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Extended_Advertising_Data command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_ext_adv_data(const sdc_hci_cmd_le_set_ext_adv_data_t * p_params);

/** @brief LE Set Extended Scan Response Data.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.55
 *
 * This command is used to provide scan response data used in scanning
 * response PDUs. This command may be issued at any time after the advertising
 * set identified by the Advertising_Handle parameter has been created using
 * the HCI_LE_Set_Extended_Advertising_Parameters command (see Section 7.8.53)
 * regardless of whether advertising in that set is enabled or disabled.
 *
 * If advertising is currently enabled for the specified advertising set, the Controller shall
 * use the new data in subsequent extended advertising events for this advertising set.
 * If an extended advertising event is in progress when this command is issued, the
 * Controller may use the old or new data for that event.
 *
 * If advertising is currently disabled for the specified advertising set, the data shall be kept
 * by the Controller and used once advertising is enabled for that set. The data shall be
 * discarded when the advertising set is removed.
 *
 * Only the significant part of the scan response data should be transmitted in the
 * advertising packets as defined in [Vol 3] Part C, Section 11.
 *
 * The Host may set the scan response data in one or more operations using the
 * Operation parameter in the command. If the combined length of the data exceeds
 * the capacity of the advertising set identified by the Advertising_Handle parameter (see
 * Section 7.8.57 LE Read Maximum Advertising Data Length command) or the amount
 * of memory currently available, all the data shall be discarded and the Controller shall
 * return the error code Memory Capacity Exceeded (0x07).
 *
 * If Operation indicates the start of new data (values 0x01 or 0x03), then any existing
 * partial or complete scan response data shall be discarded.
 *
 * If the scan response data is discarded by the command or the combined length of
 * the data after the command completes is zero, the advertising set will have no scan
 * response data.
 *
 * The Fragment_Preference parameter provides a hint to the Controller as to whether
 * advertising data should be fragmented.
 * If the advertising set is non-scannable and the Host uses this command other than to
 * discard existing data, the Controller shall return the error code Invalid HCI Command
 * Parameters (0x12). If the advertising set uses scannable legacy advertising PDUs and
 * either Operation is not 0x03 or the Scan_Response_Data_Length parameter exceeds
 * 31 octets, the Controller shall return the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If Operation is not 0x03 and Scan_Response_Data_Length is zero, the Controller shall
 * return the error code Invalid HCI Command Parameters (0x12).
 *
 * If advertising is currently enabled for the specified advertising set and Operation does
 * not have the value 0x03, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * If the advertising set uses extended advertising and the combined length of the data is
 * greater than the maximum that the Controller can transmit within the longest possible
 * auxiliary advertising segment consistent with the current parameters of the advertising
 * set (using the current advertising interval if advertising is enabled), all the data shall
 * be discarded and the Controller shall return the error code Packet Too Long (0x45). If
 * advertising on the LE Coded PHY, the S=8 coding shall be assumed unless the current
 * advertising parameters require the use of S=2 for an advertising physical channel, in
 * which case the S=2 coding shall be assumed for that advertising physical channel.
 *
 * If the advertising set uses scannable extended advertising PDUs, advertising is
 * currently enabled for the specified advertising set, and Scan_Response_Data_Length
 * is zero, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * then the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Extended_Scan_Response_Data command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_ext_scan_response_data(const sdc_hci_cmd_le_set_ext_scan_response_data_t * p_params);

/** @brief LE Set Extended Advertising Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.56
 *
 * This command is used to request the Controller to enable or disable one or more
 * advertising sets using the advertising sets identified by the Advertising_Handle[i]
 * parameter. The Controller manages the timing of advertisements in accordance with the
 * advertising parameters given in the HCI_LE_Set_Extended_Advertising_Parameters
 * command. The Num_Sets parameter is the number of advertising sets contained in
 * the parameter arrays. If Enable and Num_Sets are both set to 0x00, then all advertising
 * sets are disabled.
 *
 * The Controller shall only start an advertising event when the corresponding
 * advertising set is enabled. The Controller shall continue advertising until all advertising
 * sets have been disabled. An advertising set shall be disabled when the Host
 * issues an HCI_LE_Set_Extended_Advertising_Enable command with the Enable
 * parameter set to 0x00 (Advertising is disabled), a connection is created using
 * that advertising set, the duration specified in the Duration[i] parameter expires, or
 * the number of extended advertising events transmitted for the set exceeds the
 * Max_Extended_Advertising_Events[i] parameter.
 *
 * The Duration[i] parameter indicates the duration for which that advertising set is
 * enabled. The duration begins at the start of the first advertising event of this advertising
 * set. The Controller should not start an extended advertising event that it cannot
 * complete within the duration.
 *
 * If the advertising is high duty cycle connectable directed advertising, then Duration[i]
 * shall be less than or equal to 1.28 seconds and shall not be equal to 0.
 *
 * The Max_Extended_Advertising_Events[i] parameter, if non-zero, indicates the
 * maximum number of extended advertising events that shall be sent prior to disabling
 * the extended advertising set even if the Duration[i] parameter has not expired.
 *
 * Duration[i] and Max_Extended_Advertising_Events[i] shall be ignored when Enable is
 * set to 0x00.
 * If the HCI_LE_Set_Extended_Advertising_Enable command is sent again for an
 * advertising set while that set is enabled, the timer used for the duration and the number
 * of events counter are reset and any change to the random address shall take effect.
 *
 * Disabling the advertising set identified by the Advertising_Handle[i] parameter does not
 * disable any periodic advertising associated with that set.
 *
 * Disabling an advertising set that is already disabled has no effect.
 *
 * If the same advertising set is identified by more than one entry in the
 * Advertising_Handle[i] arrayed parameter, then the Controller shall return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * If the advertising set corresponding to the Advertising_Handle[i] parameter does not
 * exist, then the Controller shall return the error code Unknown Advertising Identifier
 * (0x42).
 *
 * The remainder of this section only applies if Enable is set to 0x01.
 *
 * If the Controller does not have enough resources to schedule all the advertising sets in
 * the command, the Controller should return an error code which should be Connection
 * Rejected due to Limited Resources (0x0D).
 *
 * If Num_Sets is set to 0x00, the Controller shall return the error code Invalid HCI
 * Command Parameters (0x12).
 *
 * If the advertising set contains partial advertising data or partial scan response data, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the advertising set uses scannable extended advertising PDUs and no scan response
 * data is currently provided, the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * If the advertising set uses connectable extended advertising PDUs and the advertising
 * data in the advertising set will not fit in the AUX_ADV_IND PDU, the Controller shall
 * return the error code Invalid HCI Command Parameters (0x12).
 *
 * Note: The maximum amount of data that will fit in the PDU depends on which options
 * are selected and on the maximum length of PDU that the Controller is able to transmit.
 *
 * If extended advertising is being used and the length of any advertising data or of
 * any scan response data is greater than the maximum that the Controller can transmit
 * within the longest possible auxiliary advertising segment consistent with the chosen
 * advertising interval, the Controller shall return the error code Packet Too Long (0x45). If
 * advertising on the LE Coded PHY, the S=8 coding shall be assumed unless the current
 * advertising parameters require the use of S=2 for an advertising physical channel, in
 * which case the S=2 coding shall be assumed for that advertising physical channel.
 *
 * If the advertising set's Own_Address_Type parameter is set to 0x00 and the device
 * does not have a public address, the Controller should return an error code which should
 * be Invalid HCI Command Parameters (0x12).
 *
 * If the advertising set's Own_Address_Type parameter is set to 0x01 and the
 * random address for the advertising set has not been initialized using the
 * HCI_LE_Set_Advertising_Set_Random_Address command, the Controller shall return
 * the error code Invalid HCI Command Parameters (0x12).
 *
 * If the advertising set's Own_Address_Type parameter is set to 0x02, the Controller's
 * resolving list did not contain a matching entry, and the device does not have a public
 * address, the Controller should return an error code which should be Invalid HCI
 * Command Parameters (0x12).
 *
 * If the advertising set's Own_Address_Type parameter is set to 0x03,
 * the Controller's resolving list did not contain a matching entry, and the
 * random address for the advertising set has not been initialized using the
 * HCI_LE_Set_Advertising_Set_Random_Address command, the Controller shall return
 * the error code Invalid HCI Command Parameters (0x12).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Extended_Advertising_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * If the Duration[i] parameter is set to a value other than 0x0000, an
 * HCI_LE_Advertising_Set_Terminated event shall be generated when the duration
 * specified in the Duration[i] parameter expires. However, if the advertising set
 * is for high duty cycle connectable directed advertising and no connection
 * is created before the duration expires, an HCI_LE_Connection_Complete or
 * HCI_LE_Enhanced_Connection_Complete event with the Status parameter set to the
 * error code Advertising Timeout (0x3C) may be generated instead of or in addition to
 * the HCI_LE_Advertising_Set_Terminated event. If the Controller generates both events,
 * they may be in either order.
 *
 * If the Max_Extended_Advertising_Events[i] parameter is set to a value other than 0x00,
 * an HCI_LE_Advertising_Set_Terminated event shall be generated when the maximum
 * number of extended advertising events has been transmitted by the Controller.
 *
 * If the advertising set is connectable and a connection gets created, an
 * HCI_LE_Connection_Complete or HCI_LE_Enhanced_Connection_Complete event
 * shall be generated followed by an HCI_LE_Advertising_Set_Terminated event with
 * the Status parameter set to 0x00. The Controller should not send any other events
 * in between these two events. If the Controller supports the LE Channel Selection
 * Algorithm #2 feature, then the HCI_LE_Advertising_Set_Terminated event may be
 * immediately preceded or followed by an HCI_LE_Channel_Selection_Algorithm event.
 *
 * Note: If this command is used to disable advertising at about the same time that
 * a connection is established or the advertising duration expires, there is a possible
 * race condition in that it is possible to receive both an HCI_LE_Connection_Complete,
 * HCI_LE_Enhanced_Connection_Complete, or HCI_LE_Advertising_Set_Terminated
 * event and the HCI_Command_Complete event for this command.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_ext_adv_enable(const sdc_hci_cmd_le_set_ext_adv_enable_t * p_params);

/** @brief LE Read Maximum Advertising Data Length.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.57
 *
 * This command is used to read the maximum length of data supported by the Controller
 * for use as advertisement data or scan response data in an advertising event or as
 * periodic advertisement data.
 *
 * Note: The maximum amount may be fragmented across multiple PDUs (see [Vol 6] Part
 * B, Section 2.3.4.9).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Maximum_Advertising_Data_Length command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_max_adv_data_length(sdc_hci_cmd_le_read_max_adv_data_length_return_t * p_return);

/** @brief LE Read Number of Supported Advertising Sets.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.58
 *
 * This command is used to read the number of advertising sets (including those already
 * created) that the advertising Controller can support at the present time.
 *
 * Note: The number of advertising sets that can be supported is not fixed and the
 * Controller can change it at any time (e.g., because the memory used to store
 * advertising sets can also be used for other purposes).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Number_of_Supported_Advertising_Sets command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_number_of_supported_adv_sets(sdc_hci_cmd_le_read_number_of_supported_adv_sets_return_t * p_return);

/** @brief LE Remove Advertising Set.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.59
 *
 * This command is used to remove an advertising set from the Controller.
 *
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * then the Controller shall return the error code Unknown Advertising Identifier (0x42). If
 * advertising or periodic advertising on the advertising set is enabled, then the Controller
 * shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Remove_Advertising_Set command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_remove_adv_set(const sdc_hci_cmd_le_remove_adv_set_t * p_params);

/** @brief LE Clear Advertising Sets.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.60
 *
 * This command is used to remove all existing advertising sets from the Controller.
 *
 * If advertising or periodic advertising is enabled on any advertising set, then the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * Note: All advertising sets are cleared on HCI reset.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Clear_Advertising_Sets command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_clear_adv_sets(void);

/** @brief LE Set Periodic Advertising Parameters [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.61
 *
 * This command is used by the Host to set the parameters for periodic advertising.
 *
 * The Advertising_Handle parameter identifies the advertising set whose periodic
 * advertising parameters are being configured. If the corresponding advertising set does
 * not already exist, then the Controller shall return the error code Unknown Advertising
 * Identifier (0x42).
 *
 * The Periodic_Advertising_Interval_Min parameter shall be less than or equal to the
 * Periodic_Advertising_Interval_Max parameter. The Periodic_Advertising_Interval_Min
 * and Periodic_Advertising_Interval_Max parameters should not be the same value to
 * enable the Controller to determine the best advertising interval given other activities.
 *
 * If the periodic advertising interval range provided by the Host (Periodic_Advertising_-
 * Interval_Min, Periodic_Advertising_Interval_Max) does not overlap with the periodic
 * advertising interval range supported by the Controller, then the Controller shall return
 * an error which should use the error code Unsupported Feature or Parameter Value
 * (0x11).
 *
 * The Periodic_Advertising_Properties parameter indicates which fields should be
 * included in the advertising packet.
 *
 * The Num_Subevents parameter identifies the number of subevents that shall be
 * transmitted for each periodic advertising event. If the Num_Subevents parameter value
 * is 0x00, then the Periodic Advertising does not have responses and the Controller
 * shall ignore the Subevent_Interval, Response_Slot_Delay, Response_Slot_Spacing,
 * and Num_Response_Slots parameters. If Num_Subevents is greater than 0, then the
 * Periodic Advertising is PAwR.
 *
 * The Subevent_Interval parameter identifies the time between the subevents
 * of PAwR. The Subevent_Interval shall be less than or equal to the
 * Periodic_Advertising_Interval_Min divided by the Num_Subevents of the advertising
 * set. If Num_Subevents is set to 1, then the Controller shall ignore Subevent_Interval
 * and uses of Subevent_Interval in the next two paragraphs shall be replaced by
 * Periodic_Advertising_Interval_Max.
 *
 * The Response_Slot_Delay parameter identifies the time between the start of the
 * advertising packet at the start of a subevent and the start of the first response slot.
 * The Response_Slot_Delay shall be less than the Subevent_Interval.
 *
 * The Response_Slot_Spacing parameter identifies the time between the start of two
 * consecutive response slots. The Response_Slot_Spacing shall be less than or equal
 * to 10 × (Subevent_Interval − Response_Slot_Delay) ÷ Num_Response_Slots. If the
 * Num_Response_Slots parameter is set to 1, then the Controller shall ignore the
 * Response_Slot_Spacing parameter.
 *
 * The Num_Response_Slots parameter identifies the number of response slots in a
 * subevent.
 *
 * If the advertising set identified by the Advertising_Handle specified scannable,
 * connectable, legacy, or anonymous advertising, the Controller shall return the error
 * code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command when periodic advertising is enabled for the specified
 * advertising set, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Advertising_Handle does not identify an advertising set that is already configured
 * for periodic advertising and the Controller is unable to support more periodic advertising
 * at present, the Controller shall return the error code Memory Capacity Exceeded (0x07).
 *
 * If the advertising set already contains periodic advertising data and the length of the
 * data is greater than the maximum that the Controller can transmit within a periodic
 * advertising interval of Periodic_Advertising_Interval_Max, the Controller shall return the
 * error code Packet Too Long (0x45). If advertising on the LE Coded PHY, the S=8 coding
 * shall be assumed unless the current advertising parameters require the use of S=2 for
 * an advertising physical channel, in which case the S=2 coding shall be assumed for that
 * advertising physical channel.
 * Missing parameters:
 *
 * When a version of this command is issued that does not include all the parameters, the
 * following values shall be used for any missing parameters:
 *
 *  Parameter                                                                      Value
 *  Num_Subevents                                                                  0
 *  Subevent_Interval                                                              ignored
 *  Response_Slot_Delay                                                            ignored
 *  Response_Slot_Spacing                                                          ignored
 *  Num_Response_Slots                                                             ignored
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Advertising_Parameters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_params(const sdc_hci_cmd_le_set_periodic_adv_params_t * p_params);

/** @brief LE Set Periodic Advertising Data.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.62
 *
 * This command is used to set the data used in periodic advertising PDUs. This
 * command may be issued at any time after the advertising set identified by the
 * Advertising_Handle parameter has been configured for periodic advertising using
 * the HCI_LE_Set_Periodic_Advertising_Parameters command (see Section 7.8.61),
 * regardless of whether periodic advertising in that set is enabled or disabled. If the
 * advertising set has not been configured for periodic advertising or has been configured
 * for Periodic Advertising with Responses, then the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * If periodic advertising is currently enabled for the specified advertising set, the
 * Controller shall use the new data in subsequent periodic advertising events for this
 * advertising set. If a periodic advertising event is in progress when this command is
 * issued, the Controller may use the old or new data for that event.
 *
 * If periodic advertising is currently disabled for the specified advertising set, the data
 * shall be kept by the Controller and used once periodic advertising is enabled for that
 * set. The data shall be discarded when the advertising set is removed.
 *
 * Only the significant part of the periodic advertising data should be transmitted in the
 * advertising packets as defined in [Vol 3] Part C, Section 11.
 *
 * The Host may set the periodic advertising data in one or more operations using the
 * Operation parameter in the command. If the combined length of the data exceeds
 * the capacity of the advertising set identified by the Advertising_Handle parameter (see
 * Section 7.8.57 LE Read Maximum Advertising Data Length command) or the amount
 * of memory currently available, all the data shall be discarded and the Controller shall
 * return the error code Memory Capacity Exceeded (0x07).
 *
 * If the combined length of the data is greater than the maximum that the Controller can
 * transmit within the current periodic advertising interval (if periodic advertising is currently
 * enabled) or the Periodic_Advertising_Interval_Max for the advertising set (if currently
 * disabled), all the data shall be discarded and the Controller shall return the error code
 * Packet Too Long (0x45). If advertising on the LE Coded PHY, the S=8 coding shall
 * be assumed unless the current advertising parameters require the use of S=2 for an
 * advertising physical channel, in which case the S=2 coding shall be assumed for that
 * advertising physical channel.
 *
 * If Operation indicates the start of new data (values 0x01 or 0x03), then any existing
 * partial or complete data shall be discarded.
 *
 * If Operation is 0x04, then the behavior is the same as if the current periodic advertising
 * data had been sent again; this can be used to cause the Advertising DID value to be
 * updated (see [Vol 6] Part B, Section 4.4.2.11).
 *
 * If Operation is 0x04 and:
 *
 * • periodic advertising is currently disabled for the advertising set;
 * • the periodic advertising set contains no data; or
 * • Advertising_Data_Length is not zero;
 *
 * then the Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If Operation is not 0x03 or 0x04 and Advertising_Data_Length is zero, then the
 * Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the periodic advertising data is discarded by the command or the combined length of
 * the data after the command completes is zero, the advertising set will have no periodic
 * advertising data.
 *
 * If periodic advertising is currently enabled for the specified advertising set and
 * Operation does not have the value 0x03 or 0x04, then the Controller shall return the
 * error code Command Disallowed (0x0C).
 *
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * then the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Advertising_Data command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_data(const sdc_hci_cmd_le_set_periodic_adv_data_t * p_params);

/** @brief LE Set Periodic Advertising Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.63
 *
 * This command is used to request the Controller to enable or disable the periodic
 * advertising for the advertising set specified by the Advertising_Handle parameter
 * (ordinary advertising is not affected).
 *
 * If the advertising set is not currently enabled (see the
 * HCI_LE_Set_Extended_Advertising_Enable command), the periodic advertising is not
 * started until the advertising set is enabled. Once the advertising set has been
 * enabled, the Controller shall continue periodic advertising until the Host issues an
 * HCI_LE_Set_Periodic_Advertising_Enable command with bit 0 of Enable set to 0
 * (periodic advertising is disabled). Disabling the advertising set has no effect on the
 * periodic advertising once the advertising set has been enabled.
 *
 * The Controller manages the timing of advertisements in accordance with the advertising
 * parameters given in the HCI_LE_Set_Periodic_Advertising_Parameters command.
 *
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * If bit 0 of Enable is set to 1 (periodic advertising is enabled) and the advertising set
 * contains partial periodic advertising data, the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * If bit 0 of Enable is set to 1 and the Host has not issued the
 * HCI_LE_Set_Periodic_Advertising_Parameters command for the advertising set, the
 * Controller shall either use vendor-specified parameters or return the error code
 * Command Disallowed (0x0C).
 *
 * If bit 0 of Enable is set to 1 and the length of the periodic advertising data is greater than
 * the maximum that the Controller can transmit within the chosen periodic advertising
 * interval, the Controller shall return the error code Packet Too Long (0x45). If advertising
 * on the LE Coded PHY, the S=8 coding shall be assumed unless the current advertising
 * parameters require the use of S=2 for an advertising physical channel, in which case
 * the S=2 coding shall be assumed for that advertising physical channel.
 *
 * If bit 0 of Enable is set to 1 and the advertising set identified by the Advertising_Handle
 * specified scannable, connectable, legacy, or anonymous advertising, the Controller shall
 * return the error code Command Disallowed (0x0C).
 * If bit 0 of Enable is set to 0 and the Controller supports the Periodic Advertising ADI
 * Support feature, then the Controller shall ignore bit 1.
 *
 * If bit 1 of Enable is set to 1 and the Controller does not support the Periodic Advertising
 * ADI Support feature, the Controller shall return an error which should use the error code
 * Unsupported Feature or Parameter Value (0x11).
 *
 * Enabling periodic advertising when it is already enabled can cause the random address
 * to change. Disabling periodic advertising when it is already disabled has no effect.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Advertising_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_enable(const sdc_hci_cmd_le_set_periodic_adv_enable_t * p_params);

/** @brief LE Set Extended Scan Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.64
 *
 * This command is used to set the extended scan parameters to be used on the
 * advertising physical channels.
 *
 * The Scanning_PHYs parameter indicates the PHY(s) on which the advertising packets
 * should be received on the primary advertising physical channel. The Host may enable
 * one or more scanning PHYs. If the Host specifies a PHY that is not supported by the
 * Controller, including a bit that is reserved for future use, it should return the error code
 * Unsupported Feature or Parameter Value (0x11). The Scan_Type[i], Scan_Interval[i],
 * and Scan_Window[i] parameters array elements are ordered in the same order as
 * the set bits in the Scanning_PHY parameter, starting from bit 0. The number of array
 * elements is determined by the number of bits set in the Scanning_PHY parameter.
 *
 * The Scan_Type[i] parameter specifies the type of scan to perform.
 *
 * The Scan_Interval[i] and Scan_Window[i] parameters are recommendations from the
 * Host on how long (Scan_Window[i]) and how frequently (Scan_Interval[i]) the Controller
 * should scan (see [Vol 6] Part B, Section 4.4.3); however the frequency and length
 * of the scan is implementation specific. If the requested scan cannot be supported by
 * the implementation, the Controller shall return the error code Invalid HCI Command
 * Parameters (0x12).
 *
 * The Own_Address_Type parameter indicates the type of address being used in the
 * scan request packets.
 *
 * If the Host issues this command when scanning is enabled in the Controller, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Controller does not support the Decision-Based Advertising Filtering feature and
 * the Host issues this command with bits 2 and 3 of Scanning_Filter_Policy set to a value
 * other than 0b00, the Controller shall return an error code which should be Unsupported
 * Feature or Parameter Value (0x11).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Extended_Scan_Parameters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_ext_scan_params(const sdc_hci_cmd_le_set_ext_scan_params_t * p_params);

/** @brief LE Set Extended Scan Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.65
 *
 * This command is used to enable or disable scanning for both legacy and extended
 * advertising PDUs.
 *
 * The Enable parameter determines whether scanning is enabled or disabled. If it is set to
 * 0x00, the remaining parameters shall be ignored.
 *
 * If Enable is set to 0x01 and the Host has not issued the
 * HCI_LE_Set_Extended_Scan_Parameters command, the Controller shall either use
 * vendor-specified parameters or return the error code Command Disallowed (0x0C).
 *
 * The Filter_Duplicates parameter controls whether the Link Layer should filter out
 * duplicate advertising reports (filtering duplicates enabled) to the Host or if the Link
 * Layer should generate advertising reports for each packet received (filtering duplicates
 * disabled). See [Vol 6] Part B, Section 4.4.3.5.
 *
 * If the Filter_Duplicates parameter is set to 0x00, all advertisements received from
 * advertisers shall be sent to the Host in advertising report events.
 *
 * If the Filter_Duplicates parameter is set to 0x01, duplicate advertisements should not be
 * sent to the Host in advertising report events until scanning is disabled.
 *
 * If the Filter_Duplicates parameter is set to 0x02, duplicate advertisements in a single
 * scan period should not be sent to the Host in advertising report events; this setting shall
 * only be used if both Period and Duration are non-zero. If Filter_Duplicates is set to 0x02
 * and either Period or Duration to zero, the Controller shall return the error code Invalid
 * HCI Command Parameters (0x12).
 *
 * If the Duration parameter is zero or both the Duration parameter and Period parameter
 * are non-zero, the Controller shall continue scanning until scanning is disabled by
 * the Host issuing an HCI_LE_Set_Extended_Scan_Enable command with the Enable
 * parameter set to 0x00 (Scanning is disabled). The Period parameter shall be ignored
 * when the Duration parameter is zero.
 * If the Duration parameter is non-zero and the Period parameter is zero, the Controller
 * shall continue scanning until the duration specified in the Duration parameter has
 * expired.
 *
 * If both the Duration and Period parameters are non-zero and the Duration is greater
 * than or equal to the Period (comparing the times, not how they are represented), the
 * Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * When the Duration and Period parameters are non-zero, the Controller shall scan for
 * the duration of the Duration parameter within a scan period specified by the Period
 * parameter. After the scan period has expired, a new scan period shall begin and
 * scanning shall begin again for the duration specified. The scan periods continue until
 * the Host disables scanning.
 *
 * If the HCI_LE_Set_Extended_Scan_Enable command with Enable set to 0x01 is sent
 * while scanning is already enabled, the timers used for duration and period are reset
 * to the new parameter values and a new scan period is started. Any change to the
 * Filter_Duplicates setting or the random address shall take effect.
 *
 * Disabling scanning when it is disabled has no effect.
 *
 * Note: The duration of a scan period refers to the time spent scanning on both the
 * primary and secondary advertising physical channels. However, expiry of the duration
 * does not prevent the Link Layer from scanning for and receiving auxiliary packets of
 * received advertisements.
 *
 * If Enable is set to 0x01, the scanning parameters' Own_Address_Type parameter is set
 * to 0x00 or 0x02, and the device does not have a public address, the Controller should
 * return an error code which should be Invalid HCI Command Parameters (0x12).
 *
 * If Enable is set to 0x01, the scanning parameters' Own_Address_Type parameter is set
 * to 0x01 or 0x03, and the random address for the device has not been initialized using
 * the HCI_LE_Set_Random_Address command, the Controller shall return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Extended_Scan_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 * Zero or more LE Extended Advertising Reports are generated by the Controller based
 * on any advertising packets received and the duplicate filtering in effect. More than
 * one advertising packet may be reported in each HCI_LE_Extended_Advertising_Report
 * event.
 *
 * At the end of a single scan (Duration non-zero but Period zero), an
 * HCI_LE_Scan_Timeout event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_ext_scan_enable(const sdc_hci_cmd_le_set_ext_scan_enable_t * p_params);

/** @brief LE Extended Create Connection [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.66
 *
 * This command is used to create a single ACL connection, with the local device in the
 * Central role, to a connectable advertiser. The command is also used to create a single
 * ACL connection between a periodic advertiser and a synchronized device.
 * If a connection is created with the local device in the Peripheral role while this command
 * is pending, then this command remains pending.
 *
 * The Advertising_Handle parameter is used to identify the periodic advertising train.
 *
 * The Subevent parameter is used to identify the subevent where a connection
 * request shall be initiated from a periodic advertising train. The Host may use
 * this subevent whether or not the Controller has requested data for it using the
 * HCI_LE_Periodic_Advertising_Subevent_Data_Request event.
 *
 * The Advertising_Handle and Subevent parameters shall be set to 0xFF if these
 * parameters are not used. If the Host sets one but not both of these to 0xFF, then the
 * Controller shall return an error which should use the error code Invalid HCI Command
 * Parameters (0x12).
 *
 * If the Advertising_Handle and Subevent parameters are set to values other than 0xFF,
 * then the Initiator_Filter_Policy, Scan_Interval[i], and Scan_Window[i] parameters shall
 * be ignored.
 *
 * The Initiator_Filter_Policy parameter is used to determine whether the Filter Accept
 * List is used and whether to process decision PDUs and other advertising PDUs. If the
 * Filter Accept List is used or the policy allows decision PDUs to be processed (or both),
 * then the device to connect to is the first one that sends an advertisement received
 * successfully by the local device and that is on the list or that is accepted by the decision
 * instructions (see Section 7.8.145), as specified by the policy. If the Filter Accept List is
 * not used, then the Peer_Address_Type and the Peer_Address parameters specify the
 * address type and address of the advertising device to connect to for advertisements not
 * using decision PDUs. If Initiator_Filter_Policy is set to 0x03, then devices on the Filter
 * Accept List shall still be processed using the decision instructions.
 *
 * The Own_Address_Type parameter indicates the type of address being used in the
 * connection request packets.
 *
 * The Peer_Address_Type parameter indicates the type of address used in the
 * connectable advertisement sent by the peer.
 *
 * The Peer_Address parameter indicates the Peer’s Public Device Address, Random
 * (static) Device Address, Non-Resolvable Private Address, or Resolvable Private
 * Address depending on the Peer_Address_Type parameter.
 *
 * The Initiating_PHYs parameter indicates the PHY(s) on which the advertising packets
 * should be received on the primary advertising physical channel and the PHYs for
 * which connection parameters have been specified. The Host may enable one or more
 * initiating PHYs. If the Host specifies a PHY that is not supported by the Controller,
 * including a bit that is reserved for future use, the latter should return the error code
 * Unsupported Feature or Parameter Value (0x11). If the Host sets Advertising_Handle to
 * a value other than 0xFF and does not include the PHY used for the specified periodic
 * advertising train in Initiating_PHYs, the Controller shall return an error which should use
 * the error code Invalid HCI Command Parameters (0x12). The array elements of the
 * arrayed parameters are ordered in the same order as the set bits in the Initiating_PHYs
 * parameter, starting from bit 0. The number of array elements is determined by the
 * number of bits set in the Initiating_PHYs parameter. When a connectable advertisement
 * is received and a connection request is sent on one PHY, scanning on any other PHYs
 * is terminated.
 *
 * The Scan_Interval[i] and Scan_Window[i] parameters are recommendations from the
 * Host on how long (Scan_Window[i]) and how frequently (Scan_Interval[i]) the Controller
 * should scan (see [Vol 6] Part B, Section 4.5.3); however the frequency and length
 * of the scan is implementation specific. If the requested scan cannot be supported by
 * the implementation, the Controller shall return the error code Invalid HCI Command
 * Parameters (0x12). If bit 1 is set in Initiating_PHYs, the values for the LE 2M PHY shall
 * be ignored.
 *
 * The Connection_Interval_Min[i] and Connection_Interval_Max[i] parameters define the
 * minimum and maximum allowed connection interval. The Connection_Interval_Min[i]
 * parameter shall not be greater than the Connection_Interval_Max[i] parameter.
 *
 * The Max_Latency[i] parameter defines the maximum allowed Peripheral latency (see
 * [Vol 6] Part B, Section 4.5.1).
 *
 * The Supervision_Timeout[i] parameter defines the link supervision timeout for the
 * connection. The Supervision_Timeout[i] in milliseconds shall be larger than (1 +
 * Max_Latency[i]) × Connection_Interval_Max[i] × 2, where Connection_Interval_Max[i]
 * is given in milliseconds (see [Vol 6] Part B, Section 4.5.2).
 *
 * The Min_CE_Length[i] and Max_CE_Length[i] parameters provide the Controller
 * with the expected minimum and maximum length of the connection events. The
 * Min_CE_Length[i] parameter shall be less than or equal to the Max_CE_Length[i]
 * parameter. The Controller is not required to use these values.
 *
 * Where the connection is made on a PHY whose bit is not set in the
 * Initiating_PHYs parameter, the Controller shall use the Connection_Interval_Min[i],
 * Connection_Interval_Max[i], Max_Latency[i], Supervision_Timeout[i], Min_CE_Length[i],
 * and Max_CE_Length[i] parameters for an implementation-chosen PHY whose bit is set
 * in the Initiating_PHYs parameter.
 *
 * If the Host issues this command when another HCI_LE_Extended_Create_Connection
 * command is pending in the Controller, the Controller shall return the error code
 * Command Disallowed (0x0C).
 * If the local device is already connected to the same device address as the advertiser
 * (including two different Resolvable Private Addresses that resolve to the same IRK),
 * then the Controller shall return an error which should use the error code Connection
 * Already Exists (0x0B).
 *
 * If the Own_Address_Type parameter is set to 0x00 and the device does not have a
 * public address, the Controller should return an error code which should be Invalid HCI
 * Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x01 and the random address for the
 * device has not been initialized using the HCI_LE_Set_Random_Address command, the
 * Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x02, the Initiator_Filter_Policy parameter
 * is set to 0x00, the Controller's resolving list did not contain a matching entry, and the
 * device does not have a public address, the Controller should return an error code which
 * should be Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x02, the Initiator_Filter_Policy parameter
 * is not set to 0x00, and the device does not have a public address, the Controller should
 * return an error code which should be Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x03, the Initiator_Filter_Policy
 * parameter is set to 0x00, the Controller's resolving list did not contain a matching
 * entry, and the random address for the device has not been initialized using the
 * HCI_LE_Set_Random_Address command, the Controller shall return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x03, the Initiator_Filter_Policy parameter
 * is not set to 0x00, and the random address for the device has not been initialized using
 * the HCI_LE_Set_Random_Address command, the Controller shall return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * If the Initiating_PHYs parameter does not have at least one bit set for a PHY allowed
 * for scanning on the primary advertising physical channel, the Controller shall return the
 * error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command and the Controller has insufficient resources to handle
 * any more connections, the Controller shall return the error code Rejected due to Limited
 * Resources (0x0D).
 *
 * If the Controller does not support the Decision-Based Advertising Filtering feature and
 * the Host issues this command with Initiator_Filter_Policy set to a value other than 0x00
 * or 0x01, the Controller shall return an error code which should be Unsupported Feature
 * or Parameter Value (0x11).
 * Missing parameters:
 *
 * When a version of this command is issued that does not include all the parameters, the
 * following values shall be used for any missing parameters:
 *
 *  Parameter                                                                   Value
 *  Advertising_Handle                                                          0xFF
 *  Subevent                                                                    0xFF
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Extended_Create_Connection command,
 * the Controller sends the HCI_Command_Status event to the Host. An
 * HCI_LE_Enhanced_Connection_Complete event shall be generated when a connection
 * is created because of this command or the connection creation procedure is
 * cancelled; until the event is generated, the command is considered pending. If
 * a connection creation is discarded, then the error code Connection Failed to be
 * Established / Synchronization Timeout (0x3E) shall be used. If a connection is created,
 * this event shall be immediately followed by an HCI_LE_Channel_Selection_Algorithm
 * event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_ext_create_conn(const sdc_hci_cmd_le_ext_create_conn_t * p_params);

/** @brief LE Periodic Advertising Create Sync.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.67
 *
 * This command is used to synchronize with a single periodic advertising train from an
 * advertiser and begin receiving periodic advertising packets.
 *
 * This command may be issued whether or not scanning is enabled and scanning may
 * be enabled and disabled (see Section 7.8.65) while this command is pending. However,
 * synchronization can only occur when scanning is enabled. While scanning is disabled,
 * no attempt to synchronize will take place.
 *
 * The Options parameter is used to determine whether the Periodic Advertiser List is
 * used, whether HCI_LE_Periodic_Advertising_Report events for this periodic advertising
 * train are initially enabled or disabled, and whether duplicate reports are filtered or not.
 * If the Periodic Advertising List is used, then the periodic advertising device to listen
 * to is the first one that sends an AUX_SYNC_IND PDU received successfully by the
 * local device, that is on the list, and that does not have the wrong type of Constant
 * Tone Extension. If the Periodic Advertiser List is not used, then the Advertising_SID,
 * Advertiser Address_Type, and Advertiser Address parameters specify the periodic
 * advertising device to listen to; otherwise they shall be ignored.
 *
 * The Advertising_SID parameter, if used, specifies the value that shall match the
 * Advertising SID subfield in the ADI field of the received advertisement for it to be used
 * to synchronize.
 *
 * The Skip parameter specifies the maximum number of consecutive periodic advertising
 * events that the receiver may skip after successfully receiving a periodic advertising
 * packet.
 *
 * The Sync_Timeout parameter specifies the maximum permitted time between
 * successful receives. If this time is exceeded, synchronization is lost.
 *
 * The Sync_CTE_Type parameter specifies whether to only synchronize to periodic
 * advertising with certain types of Constant Tone Extension (a value of 0 indicates that
 * the presence or absence of a Constant Tone Extension is irrelevant). If the periodic
 * advertising has the wrong type of Constant Tone Extension then:
 *
 * • If bit 0 of Options is set, the Controller shall ignore this address and SID, as if they
 *   were not on the Periodic Advertiser List, and continue to search for other periodic
 *   advertisements.
 * • Otherwise, the Controller shall cancel the synchronization with the error code
 *   Unsupported Remote Feature (0x1A).
 *
 * If the Controller does not support the Connectionless CTE Receiver feature, then the
 * Host should set Sync_CTE_Type to 0.
 *
 * If the periodic advertiser changes the type of Constant Tone Extension after the scanner
 * has synchronized with the periodic advertising, the scanner's Link Layer shall remain
 * synchronized.
 *
 * If the Host issues this command with the Sync_CTE_Type parameter set to any value
 * other than zero but the Controller does not support the Connectionless CTE Receiver
 * feature, then the Controller should return an error which should use the error code
 * Unsupported Feature or Parameter Value (0x11). If the Controller does not return an
 * error, then it should behave as if Sync_CTE_Type is set to zero.
 *
 * If the Host sets all the non-reserved bits of the Sync_CTE_Type parameter to 1, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * Irrespective of the value of the Skip parameter, the Controller should stop skipping
 * packets before the Sync_Timeout would be exceeded.
 *
 * If the Host issues this command when another
 * HCI_LE_Periodic_Advertising_Create_Sync command is pending, the Controller shall
 * return the error code Command Disallowed (0x0C).
 *
 * If the Host issues this command with bit 0 of Options not set and with Advertising_SID,
 * Advertiser_Address_Type, and Advertiser_Address the same as those of a periodic
 * advertising train that the Controller is already synchronized to, the Controller shall return
 * the error code Connection Already Exists (0x0B).
 *
 * If the Host issues this command and the Controller has insufficient resources to handle
 * any more periodic advertising trains, the Controller shall return the error code Memory
 * Capacity Exceeded (0x07).
 *
 * If bit 1 of Options is set to 1 and the Controller supports the Periodic Advertising ADI
 * Support feature, then the Controller shall ignore bit 2.
 * If bit 1 of Options is set to 0, bit 2 is set to 1, and the Controller does not support the
 * Periodic Advertising ADI Support feature, then the Controller shall return an error which
 * should use the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If bit 1 of the Options parameter is set to 1 and the Controller does not support
 * the HCI_LE_Set_Periodic_Advertising_Receive_Enable command, the Controller shall
 * return the error code Connection Failed to be Established / Synchronization Timeout
 * (0x3E).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Periodic_Advertising_Create_Sync command has been received,
 * the Controller sends the HCI_Command_Status event to the Host. An
 * HCI_LE_Periodic_Advertising_Sync_Established event shall be generated when the
 * Controller starts receiving periodic advertising packets.
 *
 * When the Controller receives periodic advertising packets then, if reporting is enabled, it
 * sends HCI_LE_Periodic_Advertising_Report events to the Host.
 *
 * If the Controller does not receive a periodic advertising packet within
 * 6 periodic advertising events of first listening, then it shall generate an
 * HCI_LE_Periodic_Advertising_Sync_Established event that should have Status set to
 * Connection Failed to be Established / Synchronization Timeout (0x3E).
 *
 * Note: The HCI_LE_Periodic_Advertising_Sync_Established event can be
 * sent as a result of synchronization being canceled by an
 * HCI_LE_Periodic_Advertising_Create_Sync_Cancel command.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_periodic_adv_create_sync(const sdc_hci_cmd_le_periodic_adv_create_sync_t * p_params);

/** @brief LE Periodic Advertising Create Sync Cancel.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.68
 *
 * This command is used to cancel the HCI_LE_Periodic_Advertising_Create_Sync
 * command while it is pending.
 *
 * If the Host issues this command while no HCI_LE_Periodic_Advertising_Create_Sync
 * command is pending, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Periodic_Advertising_Create_Sync_Cancel command has
 * completed, the Controller sends an HCI_Command_Complete event to the Host.
 *
 * After the HCI_Command_Complete is sent and if the cancellation was successful, the
 * Controller sends an HCI_LE_Periodic_Advertising_Sync_Established event to the Host
 * with the error code Operation Cancelled by Host (0x44).
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_periodic_adv_create_sync_cancel(void);

/** @brief LE Periodic Advertising Terminate Sync.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.69
 *
 * This command is used to stop reception of the periodic advertising train identified by the
 * Sync_Handle parameter.
 *
 * If the periodic advertising train corresponding to the Sync_Handle parameter does not
 * exist, then the Controller shall return the error code Unknown Advertising Identifier
 * (0x42).
 *
 * Following successful completion of this command the Sync_Handle is destroyed.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Periodic_Advertising_Terminate_Sync command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_periodic_adv_terminate_sync(const sdc_hci_cmd_le_periodic_adv_terminate_sync_t * p_params);

/** @brief LE Add Device To Periodic Advertiser List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.70
 *
 * This command is used to add an entry, consisting of a single device address and SID,
 * to the Periodic Advertiser list stored in the Controller. Any additions to the Periodic
 * Advertiser list take effect immediately. If the entry is already on the list, the Controller
 * shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command when an HCI_LE_Periodic_Advertising_Create_Sync
 * command is pending, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * When a Controller cannot add an entry to the Periodic Advertiser list because the list is
 * full, the Controller shall return the error code Memory Capacity Exceeded (0x07).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Add_Device_To_Periodic_Advertiser_List command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_add_device_to_periodic_adv_list(const sdc_hci_cmd_le_add_device_to_periodic_adv_list_t * p_params);

/** @brief LE Remove Device From Periodic Advertiser List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.71
 *
 * This command is used to remove one entry from the list of Periodic Advertisers stored
 * in the Controller. Removals from the Periodic Advertisers List take effect immediately.
 *
 * If the Host issues this command when an HCI_LE_Periodic_Advertising_Create_Sync
 * command is pending, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * When a Controller cannot remove an entry from the Periodic Advertiser list because it
 * is not found, the Controller shall return the error code Unknown Advertising Identifier
 * (0x42).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Remove_Device_From_Periodic_Advertiser_List command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_remove_device_from_periodic_adv_list(const sdc_hci_cmd_le_remove_device_from_periodic_adv_list_t * p_params);

/** @brief LE Clear Periodic Advertiser List.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.72
 *
 * This command is used to remove all entries from the list of Periodic Advertisers in the
 * Controller.
 *
 * If this command is used when an HCI_LE_Periodic_Advertising_Create_Sync
 * command is pending, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Clear_Periodic_Advertiser_List command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_clear_periodic_adv_list(void);

/** @brief LE Read Periodic Advertiser List Size.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.73
 *
 * This command is used to read the number of Periodic Advertiser list entries (including
 * those already stored there) that the Controller can store at the present time.
 *
 * Note: The number of entries that can be stored is not fixed and the Controller can
 * change it at any time (e.g., because the memory used to store the list can also be used
 * for other purposes).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Periodic_Advertiser_List_Size command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_periodic_adv_list_size(sdc_hci_cmd_le_read_periodic_adv_list_size_return_t * p_return);

/** @brief LE Read Transmit Power.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.74
 *
 * This command is used to read the minimum and maximum transmit powers supported
 * by the Controller across all supported PHYs.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read _Transmit_Power command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_transmit_power(sdc_hci_cmd_le_read_transmit_power_return_t * p_return);

/** @brief LE Read RF Path Compensation.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.75
 *
 * This command is used to read the RF path compensation value parameters used in the
 * Tx power level and RSSI calculation.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_RF_Path_Compensation command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_rf_path_compensation(sdc_hci_cmd_le_read_rf_path_compensation_return_t * p_return);

/** @brief LE Write RF Path Compensation.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.76
 *
 * This command is used to indicate the RF path gain or loss between the RF
 * transceiver and the antenna contributed by intermediate components. A positive
 * value means a net RF path gain and a negative value means a net RF path loss.
 * The RF_TX_Path_Compensation_Value parameter shall be used by the Controller to
 * calculate the radiative Tx power level used in HCI commands, HCI events, Advertising
 * physical channel PDUs, and Link Layer Control PDUs using the following equation:
 *
 * Radiative Tx power level = Tx power level at RF transceiver output +
 * RF_TX_Path_Compensation_Value
 *
 * For example, if the Tx power level is +4 (dBm) at RF transceiver output and the
 * RF_TX_Path_Compensation_Value is -1.5 (dB), the radiative Tx power level is +4+
 * (-1.5) = 2.5 (dBm).
 *
 * The RF_RX_Path_Compensation_Value parameter shall be used by the Controller to
 * calculate the RSSI value reported to the Host using the following equation:
 *
 * Rx power level at RF transceiver input = Rx power level at antenna +
 * RF_RX_Path_Compensation_Value
 *
 * For example, if the Rx power level is -45 (dBm) at RF transceiver input and the
 * RF_RX_Path_Compensation_Value is -3.2 (dB), the Rx power level at antenna is -41.8
 * (dBm).
 *
 * The default values for the RF path compensation are vendor-specific.
 *
 * This command can be issued at any time. If this command is issued during an ongoing
 * over-the-air RF activity, the Controller may apply the Tx path compensation immediately
 * or after a vendor-specific delay.
 *
 * The Controller shall apply a change to the Tx path compensation value either by leaving
 * the power at the transceiver output unchanged and altering the radiative Tx power level
 * or by altering the power at the transceiver output to maintain any previously chosen
 * radiative Tx power level.
 *
 * If the Host needs to maintain a specific radiative transmit power level for an advertising
 * set, it should disable that set before issuing this command then, after the command
 * completes, reissue the HCI_LE_Set_Extended_Advertising_Parameters command for
 * that set and then re-enable it.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Write_RF_Path_Compensation command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * If the command leads to a change in the local radiative transmit power
 * level for an LE ACL connection, then the Controller shall generate an
 * HCI_LE_Transmit_Power_Reporting event if local reporting is enabled and shall initiate
 * a Link Layer Power Change Indication procedure (see [Vol 6] Part B, Section 5.1.18) if
 * remote reporting is enabled.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_write_rf_path_compensation(const sdc_hci_cmd_le_write_rf_path_compensation_t * p_params);

/** @brief LE Set Privacy Mode.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.77
 *
 * This command is used to allow the Host to specify the privacy mode to be used for a
 * given entry on the resolving list. The effect of this setting is specified in [Vol 6] Part B,
 * Section 4.7.
 *
 * When an entry on the resolving list is removed, the mode associated with that entry
 * shall also be removed.
 *
 * This command shall not be used when address resolution is enabled in the Controller
 * and:
 *
 * • Advertising (other than periodic advertising) is enabled,
 * • Scanning is enabled, or
 * • an HCI_LE_Create_Connection, HCI_LE_Extended_Create_Connection, or
 *   HCI_LE_Periodic_Advertising_Create_Sync command is pending.
 *
 * This command may be used at any time when address resolution is disabled in the
 * Controller.
 *
 * If the device is not on the resolving list, the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Privacy_Mode command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_privacy_mode(const sdc_hci_cmd_le_set_privacy_mode_t * p_params);

/** @brief LE Receiver Test [v3].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.28
 *
 * This command is used to start a test where the IUT receives test reference packets at a
 * fixed interval. The tester generates the test reference packets.
 *
 * The RX_Channel and PHY parameters specify the RF channel and PHY to be used by
 * the receiver. If the Host sets the PHY parameter to a PHY that the Controller does not
 * support, including a value that is reserved for future use, the Controller shall return the
 * error code Unsupported Feature or Parameter Value (0x11).
 *
 * The Modulation_Index parameter specifies whether or not the Controller should assume
 * the receiver has a stable modulation index.
 *
 * The Expected_CTE_Length and Expected_CTE_Type parameters specify the expected
 * length and type of the Constant Tone Extensions in received test reference packets.
 * When receiving on a PHY that allows Constant Tone Extensions, if the Constant Tone
 * Extension in a received test reference packet does not match both of these, the IUT
 * shall discard that packet. If Expected_CTE_Length is not zero and PHY specifies a PHY
 * that does not allow Constant Tone Extensions, the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * If the Slot_Durations parameter is set to 0x01 and the Controller does not support 1 µs
 * switching and sampling, the Controller shall return the error code Unsupported Feature
 * or Parameter Value (0x11).
 * Slot_Durations, Switching_Pattern_Length, and Antenna_IDs[i] are only used when
 * expecting an AoA Constant Tone Extension and shall be ignored when expecting an
 * AoD Constant Tone Extension.
 *
 * If the Controller determines that any of the Antenna_IDs[i] values do not identify an
 * antenna in the device's antenna array, it shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * Note: Some Controllers may be unable to determine which values do or do not identify
 * an antenna.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Receiver_Test command has completed, an HCI_Command_-
 * Complete event shall be generated.
 *
 * If the Expected_CTE_Length parameter is not set to zero, then HCI_LE_-
 * Connectionless_IQ_Report events may be generated by the Controller.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_receiver_test_v3(const sdc_hci_cmd_le_receiver_test_v3_t * p_params);

/** @brief LE Transmitter Test [v3].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.29
 *
 * This command is used to start a test where the IUT generates test reference packets
 * at a fixed interval. The Controller shall transmit at the power level indicated by the
 * TX_Power_Level parameter.
 *
 * The TX_Channel and PHY parameters specify the RF channel and PHY to be used by
 * the transmitter. If the Host sets the PHY parameter to a PHY that the Controller does
 * not support, including a value that is reserved for future use, the Controller shall return
 * the error code Unsupported Feature or Parameter Value (0x11).
 *
 * The Test_Data_Length and Packet_Payload parameters specify the length and
 * contents of the Payload of the test reference packets. An LE Controller supporting
 * the HCI_LE_Transmitter_Test command shall support Packet_Payload values 0x00,
 * 0x01 and 0x02. An LE Controller supporting the LE Coded PHY shall also
 * support Packet_Payload value 0x04. An LE Controller may support other values of
 * Packet_Payload.
 *
 * The CTE_Length and CTE_Type parameters specify the length and type of the
 * Constant Tone Extension in the test reference packets. If the CTE_Type parameter
 * is set to 0x01 and the Controller does not support 1 µs switching, the Controller shall
 * return the error code Unsupported Feature or Parameter Value (0x11). If CTE_Length is
 * not zero and PHY specifies a PHY that does not allow Constant Tone Extensions, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * The Switching_Pattern_Length and Antenna_IDs[i] parameters specify the antenna
 * switching pattern. They are only used when transmitting an AoD Constant Tone
 * Extension and shall be ignored when transmitting an AoA Constant Tone Extension.
 *
 * If the Controller determines that any of the Antenna_IDs[i] values do not identify an
 * antenna in the device's antenna array, it shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * Note: Some Controllers may be unable to determine which values do or do not identify
 * an antenna.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Transmitter_Test command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_transmitter_test_v3(const sdc_hci_cmd_le_transmitter_test_v3_t * p_params);

/** @brief LE Set Connectionless CTE Transmit Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.80
 *
 * This command is used to set the type, length, and antenna switching pattern for the
 * transmission of Constant Tone Extensions in any periodic advertising on the advertising
 * set identified by the Advertising_Handle parameter.
 *
 * The CTE_Count parameter specifies how many packets with a Constant Tone
 * Extension are to be transmitted in each periodic advertising event. If the number of
 * packets that would otherwise be transmitted is less than this, the Controller shall
 * transmit sufficient AUX_CHAIN_IND PDUs with no AdvData to make up the number.
 * However, if a change in circumstances since this command was issued means that the
 * Controller can no longer schedule all of these packets, it should transmit as many as
 * possible.
 *
 * If the Host issues this command when Constant Tone Extensions have been enabled
 * in the advertising set, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * The Switching_Pattern_Length and Antenna_IDs[i] parameters are only used when
 * transmitting an AoD Constant Tone Extension and shall be ignored if CTE_Type
 * specifies an AoA Constant Tone Extension.
 *
 * If the CTE_Length parameter is greater than the maximum length of Constant Tone
 * Extension supported, the Controller shall return the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * If the Host requests a type of Constant Tone Extension that the Controller does not
 * support, the Controller shall return the error code Unsupported Feature or Parameter
 * Value (0x11).
 *
 * If the Controller is unable to schedule CTE_Count packets in each event, the Controller
 * shall return the error code Unsupported Feature or Parameter Value (0x11).
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * If Switching_Pattern_Length is greater than the maximum length of switching pattern
 * supported by the Controller (see Section 7.8.87), the Controller shall return the error
 * code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Controller determines that any of the Antenna_IDs[i] values do not identify an
 * antenna in the device's antenna array, it shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * Note: Some Controllers may be unable to determine which values do or do not identify
 * an antenna.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Connectionless_CTE_Transmit_Parameters command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_connless_cte_transmit_params(const sdc_hci_cmd_le_set_connless_cte_transmit_params_t * p_params);

/** @brief LE Set Connectionless CTE Transmit Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.81
 *
 * This command is used to request that the Controller enables or disables the use of
 * Constant Tone Extensions in any periodic advertising on the advertising set identified by
 * Advertising_Handle.
 *
 * In order to start sending periodic advertisements containing a Constant
 * Tone Extension, the Host must also enable periodic advertising using the
 * HCI_LE_Set_Periodic_Advertising_Enable command (see Section 7.8.63).
 *
 * Note: Periodic advertising can only be enabled when advertising is enabled on the
 * same advertising set, but can continue after advertising has been disabled.
 *
 * If the Host issues this command before it has issued the
 * HCI_LE_Set_Periodic_Advertising_Parameters command (see Section 7.8.61) for the
 * advertising set, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Once enabled, the Controller shall continue advertising with Constant Tone Extensions
 * until either one of the following occurs:
 *
 * • The Host issues an HCI_LE_Set_Connectionless_CTE_Transmit_Enable command
 *   with CTE_Enable set to 0x00 (disabling Constant Tone Extensions but allowing
 *   periodic advertising to continue).
 * • The Host issues an HCI_LE_Set_Periodic_Advertising_Enable command (see
 *   Section 7.8.63) with Enable set to 0x00 (disabling periodic advertising). If periodic
 *   advertising is re-enabled then it shall continue to contain Constant Tone Extensions.
 *
 * If the Host issues this command before it has issued the
 * HCI_LE_Set_Connectionless_CTE_Transmit_Parameters command for the advertising
 * set, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the periodic advertising is on a PHY that does not allow Constant Tone Extensions,
 * the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * The Host may issue this command when advertising or periodic advertising is enabled
 * in the advertising set.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Connectionless_CTE_Transmit_Enable command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_connless_cte_transmit_enable(const sdc_hci_cmd_le_set_connless_cte_transmit_enable_t * p_params);

/** @brief LE Set Connection CTE Transmit Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.84
 *
 * This command is used to set the antenna switching pattern and permitted Constant
 * Tone Extension types used for transmitting Constant Tone Extensions requested by the
 * peer device on the connection identified by the Connection_Handle parameter.
 *
 * If the Host issues this command when Constant Tone Extension responses have
 * been enabled on the connection, the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * If the CTE_Types parameter has a bit set for a type of Constant Tone Extension that
 * the Controller does not support, the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * The Switching_Pattern_Length and Antenna_IDs[i] parameters are only used when
 * transmitting an AoD Constant Tone Extension and shall be ignored when CTE_Types
 * does not have a bit set for an AoD Constant Tone Extension; they do not affect the
 * transmission of an AoA Constant Tone Extension.
 *
 * If Switching_Pattern_Length is greater than the maximum length of switching pattern
 * supported by the Controller, the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * If the Controller determines that any of the Antenna_IDs[i] values do not identify an
 * antenna in the device's antenna array, it shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * Note: Some Controllers may be unable to determine which values do or do not identify
 * an antenna.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Connection_CTE_Transmit_Parameters command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_conn_cte_transmit_params(const sdc_hci_cmd_le_set_conn_cte_transmit_params_t * p_params,
                                                    sdc_hci_cmd_le_set_conn_cte_transmit_params_return_t * p_return);

/** @brief LE Connection CTE Response Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.86
 *
 * This command is used to request the Controller to respond to LL_CTE_REQ PDUs with
 * LL_CTE_RSP PDUs on the specified connection.
 *
 * If the Host issues this command before issuing the
 * HCI_LE_Set_Connection_CTE_Transmit_Parameters command at least once on the
 * connection, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host issues this command when the transmitter PHY for the connection is not a
 * PHY that allows Constant Tone Extensions, the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * If the transmitter PHY for the connection changes to a PHY that does not allow
 * Constant Tone Extensions, then the Controller shall automatically disable Constant
 * Tone Extension responses.
 *
 * Note: If the PHY changes back to a PHY that allows Constant Tone Extensions, then
 * the Controller will not automatically re-enable Constant Tone Extension responses.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Connection_CTE_Response_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_conn_cte_response_enable(const sdc_hci_cmd_le_conn_cte_response_enable_t * p_params,
                                                sdc_hci_cmd_le_conn_cte_response_enable_return_t * p_return);

/** @brief LE Read Antenna Information.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.87
 *
 * This command allows the Host to read the switching rates, the sampling rates,
 * the number of antennae, and the maximum length of a transmitted Constant Tone
 * Extension supported by the Controller.
 *
 * If the Controller does not support antenna switching, the value of
 * Max_Switching_Pattern_Length shall still be valid but will not be used by the Host.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Antenna_Information command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_antenna_information(sdc_hci_cmd_le_read_antenna_information_return_t * p_return);

/** @brief LE Set Periodic Advertising Receive Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.88
 *
 * This command will enable or disable reports for the periodic advertising train identified
 * by the Sync_Handle parameter.
 *
 * The Enable parameter determines whether reporting and duplicate filtering are enabled
 * or disabled. If the value is the same as the current state, the command has no effect.
 *
 * If the periodic advertising train corresponding to the Sync_Handle parameter does not
 * exist, the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * If the Host sets both bits 0 and 1 of Enable and the Controller does not support the
 * Periodic Advertising ADI Support feature, then the Controller shall return an error which
 * should use the error code Unsupported Feature or Parameter Value (0x11).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Advertising_Receive_Enable command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_receive_enable(const sdc_hci_cmd_le_set_periodic_adv_receive_enable_t * p_params);

/** @brief LE Periodic Advertising Sync Transfer.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.89
 *
 * This command is used to instruct the Controller to send synchronization information
 * about the periodic advertising train identified by the Sync_Handle parameter to a
 * connected device.
 *
 * The Service_Data parameter is a value provided by the Host for use by the Host of the
 * peer device. It is not used by the Controller.
 *
 * The connected device is identified by the Connection_Handle parameter.
 *
 * If the periodic advertising train corresponding to the Sync_Handle parameter does not
 * exist, the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * If the Connection_Handle parameter does not identify a current connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the remote device has not indicated support for the Periodic Advertising Sync Transfer
 * - Recipient feature, the Controller shall return the error code Unsupported Remote
 * Feature (0x1A).
 *
 * Note: This command may successfully complete after the periodic advertising
 * synchronization information is queued for transmission but before it is actually sent.
 * No indication is given as to how the recipient handled the information.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Periodic_Advertising_Sync_Transfer command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_periodic_adv_sync_transfer(const sdc_hci_cmd_le_periodic_adv_sync_transfer_t * p_params,
                                                  sdc_hci_cmd_le_periodic_adv_sync_transfer_return_t * p_return);

/** @brief LE Periodic Advertising Set Info Transfer.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.90
 *
 * This command is used to instruct the Controller to send synchronization information
 * about the periodic advertising in an advertising set to a connected device.
 *
 * The Advertising_Handle parameter identifies the advertising set. If the parameters in
 * the advertising set have changed since the periodic advertising was first enabled, the
 * current parameters – not the original ones – are sent.
 *
 * The Service_Data parameter is a value provided by the Host to identify the periodic
 * advertising train to the peer device. It is not used by the Controller.
 *
 * The connected device is identified by the Connection_Handle parameter.
 *
 * If the advertising set corresponding to the Advertising_Handle parameter does not exist,
 * the Controller shall return the error code Unknown Advertising Identifier (0x42).
 *
 * If periodic advertising is not currently in progress for the advertising set, the Controller
 * shall return the error code Command Disallowed (0x0C).
 *
 * If the Connection_Handle parameter does not identify a current connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the remote device has not indicated support for the Periodic Advertising Sync Transfer
 * - Recipient feature, the Controller shall return the error code Unsupported Remote
 * Feature (0x1A).
 *
 * Note: This command may successfully complete after the periodic advertising
 * synchronization information is queued for transmission but before it is actually sent.
 * No indication is given as to how the recipient handled the information.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Periodic_Advertising_Set_Info_Transfer command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_periodic_adv_set_info_transfer(const sdc_hci_cmd_le_periodic_adv_set_info_transfer_t * p_params,
                                                      sdc_hci_cmd_le_periodic_adv_set_info_transfer_return_t * p_return);

/** @brief LE Set Periodic Advertising Sync Transfer Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.91
 *
 * This command is used to specify how the Controller will process periodic
 * advertising synchronization information received from the device identified by the
 * Connection_Handle parameter (the "transfer mode").
 *
 * The Mode parameter specifies the action to be taken when periodic advertising
 * synchronization information is received. If Mode is 0x00, the Controller will ignore the
 * information. Otherwise it will notify the Host and synchronize to the periodic advertising.
 * Mode also specifies whether periodic advertising reports are initially enabled or disabled
 * and whether duplicates are filtered.
 *
 * The Skip parameter specifies the number of consecutive periodic advertising packets
 * that the receiver may skip after successfully receiving a periodic advertising packet.
 *
 * The Sync_Timeout parameter specifies the maximum permitted time between
 * successful receives. If this time is exceeded, synchronization is lost.
 *
 * Irrespective of the value of the Skip parameter, the Controller should stop skipping
 * packets before the Sync_Timeout would be exceeded.
 *
 * The CTE_Type parameter specifies whether to only synchronize to periodic advertising
 * with certain types of Constant Tone Extension. If the periodic advertiser changes the
 * type of the Constant Tone Extension after the Controller has synchronized with the
 * periodic advertising, it shall remain synchronized.
 *
 * Note: A value of 0 (i.e. all bits clear) indicates that the presence or absence of a
 * Constant Tone Extension is irrelevant.
 *
 * This command does not affect any processing of any periodic advertising
 * synchronization information already received from the peer device, whether or not the
 * Controller has yet synchronized to the periodic advertising train it describes.
 *
 * The parameter values provided by this command override those provided
 * via the HCI_LE_Set_Default_Periodic_Advertising_Sync_Transfer_Parameters
 * command (Section 7.8.92) or any preferences previously set using the
 * HCI_LE_Set_Periodic_Advertising_Sync_Transfer_Parameters command on the same
 * connection.
 *
 * If the Connection_Handle parameter does not identify a current connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the Host sets Mode to 0x03 and the Controller does not support the Periodic
 * Advertising ADI Support feature, then the Controller shall return an error which should
 * use the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Host sets all the non-reserved bits of CTE_Type to 1, then the Controller should
 * return an error using the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Advertising_Sync_Transfer_Parameters command
 * has completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_sync_transfer_params(const sdc_hci_cmd_le_set_periodic_adv_sync_transfer_params_t * p_params,
                                                             sdc_hci_cmd_le_set_periodic_adv_sync_transfer_params_return_t * p_return);

/** @brief LE Set Default Periodic Advertising Sync Transfer Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.92
 *
 * This command is used to specify the initial value for the
 * mode, skip, timeout, and Constant Tone Extension type (set by
 * the HCI_LE_Set_Periodic_Advertising_Sync_Transfer_Parameters command; see
 * Section 7.8.91) to be used for all subsequent connections over the LE transport.
 *
 * The Mode parameter specifies the initial action to be taken. If Mode is 0x00, the
 * Controller will ignore the information. Otherwise it will notify the Host and synchronize
 * to the periodic advertising. Mode also specifies whether periodic advertising reports are
 * initially enabled or disabled and whether duplicates are filtered.
 *
 * The Skip parameter specifies the number of consecutive periodic advertising packets
 * that the receiver may skip after successfully receiving a periodic advertising packet.
 *
 * The Sync_Timeout parameter specifies the maximum permitted time between
 * successful receives. If this time is exceeded, synchronization is lost.
 *
 * The CTE_Type parameter specifies whether to only synchronize to periodic advertising
 * with certain types of Constant Tone Extension. If the periodic advertiser changes the
 * type of the Constant Tone Extension after the Controller has synchronized with the
 * periodic advertising, it shall remain synchronized.
 *
 * Note: A value of 0 (i.e. all bits clear) indicates that the presence or absence of a
 * Constant Tone Extension is irrelevant.
 *
 * This command does not affect any existing connection.
 *
 * If the Host sets Mode to 0x03 and the Controller does not support the Periodic
 * Advertising ADI Support feature, then the Controller shall return an error which should
 * use the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Host sets all the non-reserved bits of CTE_Type to 1, then the Controller should
 * return an error using the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Default_Periodic_Advertising_Sync_Transfer_Parameters
 * command has completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_default_periodic_adv_sync_transfer_params(const sdc_hci_cmd_le_set_default_periodic_adv_sync_transfer_params_t * p_params);

/** @brief LE Read Buffer Size [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.2
 *
 * This command is used to read the maximum size of the data portion of ACL data
 * packets and isochronous data packets sent from the Host to the Controller. The Host
 * shall fragment the data transmitted to the Controller according to these values so that
 * the HCI ACL Data packets and HCI ISO Data packets will contain data up to this size
 * (“data” includes optional fields in the HCI ISO Data packet, such as ISO_SDU_Length).
 * The HCI_LE_Read_Buffer_Size command also returns the total number of HCI LE ACL
 * Data packets and isochronous data packets that can be stored in the data buffers of the
 * Controller. The HCI_LE_Read_Buffer_Size command shall be issued by the Host before
 * it sends any data to an LE Controller (see Section 4.1.1). If the Controller supports HCI
 * ISO Data packets, it shall return non-zero values for the ISO_Data_Packet_Length and
 * Total_Num_ISO_Data_Packets parameters.
 *
 * If the Controller returns a length value of zero for ACL data packets, the Host shall use
 * the HCI_Read_Buffer_Size command to determine the size of the data buffers (shared
 * between BR/EDR and LE transports).
 *
 * Note: Both the HCI_Read_Buffer_Size command and the HCI_LE_Read_Buffer_Size
 * command may return buffer length and number of packets parameter values that are
 * nonzero. This allows a Controller to offer different buffers and number of buffers for
 * BR/EDR data packets and LE data packets.
 *
 * The LE_ACL_Data_Packet_Length parameter shall be used to determine the maximum
 * size of the L2CAP PDU fragments that are contained in ACL data packets, and which
 * are transferred from the Host to the Controller to be broken up into packets by the Link
 * Layer. The Total_Num_LE_ACL_Data_Packets parameter contains the total number of
 * HCI ACL Data packets that can be stored in the data buffers of the Controller. The Host
 * determines how to divide the buffers between different connection handles.
 * The ISO_Data_Packet_Length parameter shall be used to determine the maximum
 * size of the SDU fragments that are contained in isochronous data packets, and which
 * are transferred from the Host to the Controller. The Total_Num_ISO_Data_Packets
 * parameter contains the total number of isochronous data packets that can be stored
 * in the data buffers of the Controller. The Host determines how to divide the buffers
 * between different connection handle(s).
 *
 * Note: The LE_ACL_Data_Packet_Length and ISO_Data_Packet_Length return
 * parameters do not include the length of the HCI ACL Data packet header or the HCI
 * ISO Data packet header respectively.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Buffer_Size command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_buffer_size_v2(sdc_hci_cmd_le_read_buffer_size_v2_return_t * p_return);

/** @brief LE Read ISO TX Sync.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.96
 *
 * This command is used to read the TX_Time_Stamp and Time_Offset of a transmitted
 * SDU identified by the Packet_Sequence_Number on a CIS or BIS identified by the
 * Connection_Handle parameter on the Central or Peripheral.
 *
 * The Packet_Sequence_Number parameter contains the sequence number of a
 * transmitted SDU.
 *
 * The TX_Time_Stamp and Time_Offset parameters are described in [Vol 6] Part G,
 * Section 3.3 and [Vol 6] Part G, Section 3.1 respectively. When the Connection_Handle
 * identifies a CIS or BIS that is transmitting unframed PDUs, the value of Time_Offset
 * returned shall be zero.
 *
 * If the Host issues this command with a connection handle that does not exist, or the
 * connection handle is not associated with a CIS or BIS, the Controller shall return the
 * error code Unknown Connection Identifier (0x02).
 *
 * If the Host issues this command on an existing connection handle for a CIS or BIS
 * that is not configured for transmitting SDUs, the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * If the Host issues this command before an SDU has been transmitted by the Controller,
 * the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_ISO_TX_Sync command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_iso_tx_sync(const sdc_hci_cmd_le_read_iso_tx_sync_t * p_params,
                                        sdc_hci_cmd_le_read_iso_tx_sync_return_t * p_return);

/** @brief LE Set CIG Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.97
 *
 * This command is used by a Central’s Host to create a CIG and to set the parameters of
 * one or more CISes that are associated with a CIG in the Controller.
 *
 * The CIG_ID parameter identifies a CIG. This parameter is allocated by the Central’s
 * Host and passed to the Peripheral’s Host through the Link Layers during the process
 * of creating a CIS. If the CIG_ID does not exist, then the Controller shall first create a
 * new CIG. Once the CIG is created (whether through this command or previously), the
 * Controller shall modify or add CIS configurations in the CIG that is identified by the
 * CIG_ID and update all the parameters that apply to the CIG.
 *
 * The SDU_Interval_C_To_P parameter specifies the time interval between the start of
 * consecutive SDUs from the Central’s Host for all the CISes in the CIG. This parameter
 * shall be ignored for all CISes that are unidirectional from Peripheral to Central.
 *
 * The SDU_Interval_P_To_C parameter specifies the time interval between the start
 * of consecutive SDUs from the Peripheral’s Host for all the CISes in the CIG. This
 * parameter shall be ignored for all CISes that are unidirectional from Central to
 * Peripheral.
 * The Worst_Case_SCA parameter shall be the worst-case sleep clock accuracy of all
 * the Peripherals that will participate in the CIG. The Host should get the sleep clock
 * accuracy from all the Peripherals before issuing this command. If the Host cannot get
 * the sleep clock accuracy from all the Peripherals, it shall set the Worst_Case_SCA
 * parameter to zero.
 *
 * Note: The Worst_Case_SCA parameter can be used by the Link Layer to better allow
 * for clock drift when scheduling the CISes in the CIG. For example, if a CIS has
 * more than two subevents, the Link Layer of the Central can set the timing of the
 * subevents such that the worst case drift in the Peripheral's clock will not exceed 2 ×
 * Sub_Interval. This prevents the Peripheral from synchronizing its timing to the wrong
 * subevent (adjacent subevents cannot be on the same channel).
 *
 * The Packing parameter indicates the preferred method of arranging subevents
 * of multiple CISes. The subevents can be arranged in Sequential or Interleaved
 * arrangement (see [Vol 6] Part B, Section 4.5.14.2). This is a recommendation to the
 * Controller which the Controller may ignore. This parameter shall be ignored when there
 * is only one CIS in the CIG.
 *
 * The Framing parameter indicates the format of the CIS Data PDUs of the specified
 * CISes' framing mode (see [Vol 6] Part G, Section 2) that the Host is requesting for the
 * CIG. The Controller may use any framing mode permitted by [Vol 6] Part G, Table 2.1
 * but shall set the framing mode of all the CISes in the CIG to the same mode. This
 * overrides any framing mode previously set for the CIG.
 *
 * The Max_Transport_Latency_C_To_P parameter contains the maximum transport
 * latency from the Central to the Peripheral, in milliseconds, as described in [Vol 6] Part
 * G, Section 3.2.1 and [Vol 6] Part G, Section 3.2.2. This parameter shall be ignored for
 * all CISes that are unidirectional from Peripheral to Central.
 *
 * The Max_Transport_Latency_P_To_C parameter contains the maximum transport
 * latency from the Peripheral to the Central, in milliseconds, as described in [Vol 6] Part
 * G, Section 3.2.1 and [Vol 6] Part G, Section 3.2.2. This parameter shall be ignored for
 * all CISes that are unidirectional from Central to Peripheral.
 *
 * The CIS_Count parameter indicates the number of CIS configurations being modified or
 * added by this command. The Controller shall set the CIS_Count return parameter equal
 * to this.
 *
 * The CIS_ID[i] parameter identifies a CIS and is set by the Central’s Host and passed to
 * the Peripheral’s Host through the Link Layers during the process of establishing a CIS.
 *
 * The Max_SDU_C_To_P[i] parameter identifies the maximum size of an SDU from the
 * Central’s Host. If the CIS is unidirectional from Peripheral to Central, this parameter
 * shall be set to 0. If a CIS configuration that is being modified has a data path set in
 * the Central to Peripheral direction and the Host has specified that Max_SDU_C_To_P[i]
 * shall be set to zero, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * The Max_SDU_P_To_C[i] parameter identifies the maximum size of an SDU from the
 * Peripheral’s Host. If the CIS is unidirectional from Central to Peripheral, this parameter
 * shall be set to 0. If a CIS configuration that is being modified has a data path set in
 * the Peripheral to Central direction and the Host has specified that Max_SDU_P_To_C[i]
 * shall be set to zero, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * The PHY_C_To_P[i] parameter identifies which PHY to use for transmission from the
 * Central to the Peripheral. The Host shall set at least one bit in this parameter and the
 * Controller shall pick a PHY from the bits that are set.
 *
 * The PHY_P_To_C[i] parameter identifies which PHY to use for transmission from the
 * Peripheral to the Central. The Host shall set at least one bit in this parameter and the
 * Controller shall pick a PHY from the bits that are set.
 *
 * The RTN_C_To_P[i] (Retransmission Number) parameter contains the number of times
 * that a CIS Data PDU should be retransmitted from the Central to Peripheral before
 * being acknowledged or flushed (irrespective of which CIS events the retransmission
 * opportunities occur in). If the CIS is unidirectional from Peripheral to Central, this
 * parameter shall be ignored. Otherwise, this parameter is a recommendation to the
 * Controller which the Controller may ignore.
 *
 * The RTN_P_To_C[i] parameter contains the number of times that a CIS Data PDU
 * should be retransmitted from the Peripheral to Central before being acknowledged or
 * flushed (irrespective of which CIS events the retransmission opportunities occur in). If
 * the CIS is unidirectional from Central to Peripheral, this parameter shall be ignored.
 * Otherwise, this parameter is a recommendation to the Controller which the Controller
 * may ignore.
 *
 * In each direction, if the Controller satisfies the recommendation, then every PDU
 * will have at least RTN+1 opportunities for transmission (assuming that the initial
 * transmission of that PDU happens at the earliest allowed subevent). The RTN value
 * indicates that the Host is recommending that the Controller selects a combination of CIS
 * parameters that satisfy the inequality:
 *
 * NSE × FT − NSE ÷ BN × BN−1 ≥ RTN + 1
 *
 * If the Status parameter is non-zero, then the state of the CIG and its CIS configurations
 * shall not be changed by the command. If the CIG did not already exist, it shall not be
 * created.
 * If Status is zero, then the Controller shall set each Connection_Handle[i] to the
 * connection handle corresponding to the CIS configuration specified in CIS_ID[i]. If the
 * same CIS_ID is being reconfigured, the same connection handle shall be returned.
 *
 * The connection handle of a CIS shall refer to the CIS when it exists and to the
 * configuration of the CIS stored in a CIG when the CIG exists but the CIS with that
 * CIS_ID does not.
 *
 * If the Host issues this command when the CIG is not in the configurable state, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host attempts to create a CIG or set parameters that exceed the maximum
 * supported resources in the Controller, the Controller shall return the error code Memory
 * Capacity Exceeded (0x07).
 *
 * If the Host attempts to set CIS parameters that exceed the maximum supported
 * connections in the Controller, the Controller shall return the error code Connection Limit
 * Exceeded (0x09).
 *
 * If the Host sets, in the PHY_C_To_P[i] or PHY_P_To_C[i] parameters, a bit for a PHY
 * that the Controller does not support, including a bit that is reserved for future use, the
 * Controller shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Controller does not support asymmetric PHYs and the Host sets PHY_C_To_P[i]
 * to a different value than PHY_P_To_C[i], the Controller shall return the error code
 * Unsupported Feature or Parameter Value (0x11).
 *
 * If the Host specifies an invalid combination of CIS parameters, the Controller shall
 * return the error code Invalid HCI Command Parameters (0x12) or the error code
 * Unsupported Feature or Parameter Value (0x11); it should return the error code Invalid
 * HCI Command Parameters (0x12).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_CIG_Parameters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_cig_params(const sdc_hci_cmd_le_set_cig_params_t * p_params,
                                      sdc_hci_cmd_le_set_cig_params_return_t * p_return);

/** @brief LE Set CIG Parameters Test.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.98
 *
 * This command should only be used for testing purposes.
 *
 * The command is used by a Central’s Host to create a CIG and to set the parameters of
 * one or more CISes that are associated with a CIG in the Controller.
 *
 * The CIG_ID parameter identifies a CIG. This parameter is allocated by the Central’s
 * Host and passed to the Peripheral’s Host through the Link Layers during the process
 * of creating a CIS. If the CIG_ID does not exist, then the Controller shall first create a
 * new CIG. Once the CIG is created (whether through this command or previously), the
 * Controller shall modify or add CIS configurations in the CIG that is identified by the
 * CIG_ID and update all the parameters that apply to the CIG.
 *
 * The SDU_Interval_C_To_P parameter specifies the time interval of periodic SDUs from
 * the Central’s Host.
 * The SDU_Interval_P_To_C parameter specifies the time interval of periodic SDUs from
 * the Peripheral’s Host.
 *
 * The FT_C_To_P parameter identifies the maximum time for a payload from the Central
 * to Peripheral to be transmitted and re-transmitted, after which it is flushed (see [Vol 6]
 * Part B, Section 4.5.13.5). This parameter is expressed in multiples of ISO_Interval.
 *
 * The FT_P_To_C parameter identifies the maximum time for a payload from the
 * Peripheral to Central to be transmitted and re-transmitted, after which it is flushed
 * (see [Vol 6] Part B, Section 4.5.13.5). This parameter is expressed in multiples of
 * ISO_Interval.
 *
 * The ISO_Interval parameter specifies the time between two consecutive CIS anchor
 * points.
 *
 * The CIS_Count parameter contains the number of CIS configurations being added or
 * modified by this command. The Controller shall set the CIS_Count return parameter
 * equal to this.
 *
 * The CIS_ID[i] parameter identifies the CIS and is set by the Central’s Host and passed
 * to the Peripheral’s Host through the Link Layers during the process of establishing a
 * CIS.
 *
 * The Worst_Case_SCA parameter is the worst-case sleep clock accuracy of all the
 * Peripherals that will participate in the CIG. The Host should get the sleep clock
 * accuracy from all the Peripherals before issuing this command. In case the Host cannot
 * get the sleep clock accuracy from all the Peripherals, it shall set the Worst_Case_SCA
 * parameter to zero.
 *
 * Note: The Worst_Case_SCA parameter can be used by the Link Layer to better allow
 * for clock drift when scheduling the CISes in the CIG. For example, if a CIS has
 * more than two subevents, the Link Layer of the Central can set the timing of the
 * subevents such that the worst case drift in the Peripheral's clock will not exceed 2 ×
 * Sub_Interval. This prevents the Peripheral from synchronizing its timing to the wrong
 * subevent (adjacent subevents cannot be on the same channel).
 *
 * The Packing parameter is used to indicate the method of arranging subevents
 * of multiple CISes. The subevents can be arranged in Sequential or Interleaved
 * arrangement. This parameter shall be ignored when there is only one CIS in the CIG.
 *
 * The Framing parameter specifies the framing mode (see [Vol 6] Part G, Section 2) that
 * the Controller shall use for all the CISes in the CIG. This overrides any framing mode
 * previously set for the CIG.
 *
 * The NSE[i] parameter identifies the maximum number of subevents for each CIS in a
 * CIG event.
 * The Max_SDU_C_To_P[i] parameter identifies the maximum size of SDU from the
 * Central’s Host. If the CIS is unidirectional from Peripheral to Central, this parameter
 * shall be set to 0. If a CIS configuration that is being modified has a data path set in
 * the Central to Peripheral direction and the Host has specified that Max_SDU_C_To_P[i]
 * shall be set to zero, the Controller shall return the error code Command Disallowed
 * (0x0C). The minimum value of the Max_SDU_Size parameter in the ISO Transmit Test
 * mode when the Payload_Type = 1 or 2 shall be 4 octets.
 *
 * The Max_SDU_P_To_C[i] parameter identifies the maximum size of SDU from the
 * Peripheral’s Host. If the CIS is unidirectional from Central to Peripheral, this parameter
 * shall be set to 0. If a CIS configuration that is being modified has a data path set in
 * the Peripheral to Central direction and the Host has specified that Max_SDU_P_To_C[i]
 * shall be set to zero, the Controller shall return the error code Command Disallowed
 * (0x0C).The minimum value of the Max_SDU parameter in the ISO Transmit Test mode
 * when the Payload_Type = 1 or 2 shall be 4 octets.
 *
 * The Max_PDU_C_To_P[i] parameter identifies the maximum size PDU from the Central
 * to Peripheral.
 *
 * The Max_PDU_P_To_C[i] parameter identifies the maximum size PDU from the
 * Peripheral to Central.
 *
 * The PHY_C_To_P[i] parameter identifies the PHY to be used for transmission of
 * packets from the Central to the Peripheral. The Host shall set only one bit in this
 * parameter and the Controller shall use the PHY set by the Host.
 *
 * The PHY_P_To_C[i] parameter identifies the PHY to be used for transmission of
 * packets from the Peripheral to the Central. The Host shall set only one bit in this
 * parameter and the Controller shall use the PHY set by the Host.
 *
 * The BN_C_To_P[i] parameter identifies the burst number for Central to Peripheral (see
 * [Vol 6] Part B, Section 4.5.13). If the CIS is unidirectional from Peripheral to Central, this
 * parameter shall be set to zero.
 *
 * The BN_P_To_C[i] parameter identifies the burst number for Peripheral to Central (see
 * [Vol 6] Part B, Section 4.5.13). If the CIS is unidirectional from Central to Peripheral, this
 * parameter shall be set to zero.
 *
 * If the Status parameter is non-zero, then the state of the CIG and its CIS configurations
 * shall not be changed by the command. If the CIG did not already exist, it shall not be
 * created.
 *
 * If Status is zero, then the Controller shall set Connection_Handle[i] to the connection
 * handle corresponding to the CIS configuration specified in CIS_ID[i]. If the same CIS_ID
 * is being reconfigured, the same connection handle shall be returned.
 * If the Host issues this command when the CIG is not in the configurable state, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host attempts to create a CIG or set parameters that exceed the maximum
 * supported resources in the Controller, the Controller shall return the error code Memory
 * Capacity Exceeded (0x07).
 *
 * If the Host attempts to set CIS parameters that exceed the maximum supported
 * connections in the Controller, the Controller shall return the error code Connection Limit
 * Exceeded (0x09).
 *
 * If the Host attempts to set an invalid combination of CIS parameters, the Controller
 * shall return the error code Invalid HCI Command Parameters (0x12) or the error code
 * Unsupported Feature or Parameter Value (0x11); it should return the error code Invalid
 * HCI Command Parameters (0x12).
 *
 * If the Host sets, in the PHY_C_To_P[i] or PHY_P_To_C[i] parameters, a bit for a PHY
 * that the Controller does not support, including a bit that is reserved for future use, the
 * Controller shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Controller does not support asymmetric PHYs and the Host sets PHY_C_To_P[i]
 * to a different value than PHY_P_To_C[i], the Controller shall return the error code
 * Unsupported Feature or Parameter Value (0x11).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_CIG_Parameters_Test command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_cig_params_test(const sdc_hci_cmd_le_set_cig_params_test_t * p_params,
                                           sdc_hci_cmd_le_set_cig_params_test_return_t * p_return);

/** @brief LE Create CIS.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.99
 *
 * This command is used by the Central’s Host to create one or more CISes using the
 * connections identified by the ACL_Connection_Handle arrayed parameter.
 *
 * The CIS_Count parameter is the total number of CISes created by this command.
 *
 * The CIS_Connection_Handle[i] parameter specifies the connection handle
 * corresponding to the configuration of the CIS to be created and whose configuration
 * is already stored in a CIG.
 *
 * The ACL_Connection_Handle[i] parameter specifies the connection handle of
 * the ACL connection associated with each CIS to be created. The list of
 * the ACL_Connection_Handles shall be in the same order as the list of
 * the CIS_Connection_Handles e.g., CIS_Connection_Handle[1] will connect to the
 * Peripheral associated with the ACL_Connection_Handle[1].
 *
 * If this command is issued on the Central before the devices have performed the
 * Feature Exchange procedure, then the Controller shall complete that procedure before
 * initiating the Connected Isochronous Stream Creation procedure (see [Vol 6] Part B,
 * Section 5.1.15).
 *
 * If any ACL_Connection_Handle[i] is not the handle of an existing ACL connection or any
 * CIS_Connection_Handle[i] is not the handle of a CIS or CIS configuration, the Controller
 * shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the Host attempts to create a CIS that has already been created, the Controller shall
 * return the error code Connection Already Exists (0x0B).
 *
 * If two different elements of the CIS_Connection_Handle arrayed parameter identify the
 * same CIS, the Controller shall return the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If the Host issues this command before all the HCI_LE_CIS_Established events from
 * the previous use of the command have been generated, the Controller shall return the
 * error code Command Disallowed (0x0C).
 * If the Host issues this command on an ACL_Connection_Handle where the Controller is
 * the Peripheral, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Note: The order of the CIS connection handles in this command does not
 * relate to the order of connection handles in the return parameters of the
 * HCI_LE_Set_CIG_Parameters command or the HCI_LE_Set_CIG_Parameters_Test
 * command.
 *
 * If the Host issues this command when the Connected Isochronous Stream (Host
 * Support) feature bit (see [Vol 6] Part B, Section 4.6.27) is not set, the Controller shall
 * return the error code Command Disallowed (0x0C).
 *
 * If the Host specified an invalid combination of parameters in the
 * HCI_LE_Set_CIG_Parameters or HCI_LE_Set_CIG_Parameters_Test command that
 * created the CIS configuration, but the Controller could not detect the problem without
 * knowing the properties of the ACL connection associated with the CIS, then the
 * Controller shall return an error which should use the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * Note: If an error is reported in the HCI_Command_Status event, it means that no
 * CIS is created and the Host cannot determine which CIS had the error. Therefore, in
 * the case of an error that only affects one CIS of several, reporting the error in the
 * HCI_LE_CIS_Established event for that CIS means that the remaining CISes are still
 * created and the Host can determine which CIS had the error.
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Create_CIS command, the Controller sends
 * the HCI_Command_Status event to the Host. An HCI_LE_CIS_Established event will
 * be generated for each CIS when it is established or if it is disconnected or considered
 * lost before being established; until all the events are generated, the command remains
 * pending.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_create_cis(const sdc_hci_cmd_le_create_cis_t * p_params);

/** @brief LE Remove CIG.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.100
 *
 * This command is used by the Central’s Host to remove the CIG identified by CIG_ID.
 *
 * The CIG_ID parameter contains the identifier of the CIG.
 *
 * This command shall delete the CIG_ID and also delete the Connection_Handles of the
 * CIS configurations stored in the CIG.
 *
 * This command shall also remove the isochronous data paths that are associated with
 * the Connection_Handles of the CIS configurations, which is equivalent to issuing the
 * HCI_LE_Remove_ISO_Data_Path command (see Section 7.8.110).
 *
 * If the Host tries to remove a CIG which is in the active state, then the Controller shall
 * return the error code Command Disallowed (0x0C).
 *
 * If the Host issues this command with a CIG_ID that does not exist, the Controller shall
 * return the error code Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Remove_CIG command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_remove_cig(const sdc_hci_cmd_le_remove_cig_t * p_params,
                                  sdc_hci_cmd_le_remove_cig_return_t * p_return);

/** @brief LE Accept CIS Request.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.101
 *
 * This command is used by the Peripheral’s Host to inform the Controller to accept the
 * request for the CIS that is identified by the Connection_Handle.
 *
 * The command shall only be issued after an HCI_LE_CIS_Request event has occurred.
 * The event contains the Connection_Handle of the CIS.
 *
 * If the Peripheral’s Host issues this command with a Connection_Handle that does not
 * exist, or the Connection_Handle is not for a CIS, the Controller shall return the error
 * code Unknown Connection Identifier (0x02).
 *
 * If the Peripheral's Host issues this command with a Connection_Handle for a CIS that
 * has already been established or that already has an HCI_LE_Accept_CIS_Request
 * or HCI_LE_Reject_CIS_Request command in progress, the Controller shall return the
 * error code Command Disallowed (0x0C).
 *
 * If the Central’s Host issues this command, the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Accept_CIS_Request command,
 * the Controller sends the HCI_Command_Status event to the Host. An
 * HCI_LE_CIS_Established event will be generated when the CIS is established or is
 * considered lost before being established.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_accept_cis_request(const sdc_hci_cmd_le_accept_cis_request_t * p_params);

/** @brief LE Reject CIS Request.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.102
 *
 * This command is used by the Peripheral’s Host to inform the Controller to reject the
 * request for the CIS that is identified by the Connection_Handle.
 *
 * The command shall only be issued after an HCI_LE_CIS_Request event has occurred.
 * The event contains the Connection_Handle of the CIS.
 *
 * When this command succeeds, the Controller shall delete the Connection_Handle of
 * the requested CIS.
 *
 * The Reason parameter indicates the reason for rejecting the CIS request.
 *
 * If the Peripheral's Host issues this command with a Connection_Handle that is not for a
 * CIS, the Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the Peripheral's Host issues this command with a Connection_Handle for a CIS that
 * has already been established or that already has an HCI_LE_Accept_CIS_Request
 * or HCI_LE_Reject_CIS_Request command in progress, the Controller shall return the
 * error code Command Disallowed (0x0C).
 *
 * If the Central’s Host issues this command, the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Reject_CIS_Request command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_reject_cis_request(const sdc_hci_cmd_le_reject_cis_request_t * p_params,
                                          sdc_hci_cmd_le_reject_cis_request_return_t * p_return);

/** @brief LE Create BIG.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.103
 *
 * This command is used to create a BIG with one or more BISes (see [Vol 6] Part B,
 * Section 4.4.6). All BISes in a BIG have the same value for all parameters.
 *
 * The BIG_Handle contains the identifier of the BIG. This parameter is allocated by the
 * Host and used by the Controller and the Host to identify a BIG.
 *
 * The Advertising_Handle identifies the associated periodic advertising train of the BIG
 * (see [Vol 6] Part B, Section 4.4.5.1).
 *
 * The Num_BIS parameter contains the total number of BISes in the BIG.
 *
 * The SDU_Interval parameter contains the time interval of the periodic SDUs.
 *
 * The Max_SDU parameter contains the maximum size of an SDU.
 *
 * The Max_Transport_Latency parameter is the maximum transport latency (in
 * milliseconds) as described in [Vol 6] Part G, Section 3.2.1 and [Vol 6] Part G,
 * Section 3.2.2. This includes pre-transmissions.
 *
 * The RTN (Retransmission Number) parameter contains the number of times every PDU
 * should be retransmitted, irrespective of which BIG events the retransmissions occur in.
 * This is a recommendation to the Controller which the Controller may ignore.
 *
 * The PHY parameter is a bit field that indicates the PHY used for transmission of PDUs
 * of BISes in the BIG. The Host shall set at least one bit in this parameter and the
 * Controller shall pick a PHY from the bits set. If the Host sets, in the PHY parameter,
 * a bit for a PHY that the Controller does not support, including a bit that is reserved
 * for future use, then the Controller shall return the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * The Packing parameter is used to indicate the preferred method of arranging subevents
 * of multiple BISes. The subevents can be arranged in Sequential or Interleaved
 * arrangement. This is a recommendation to the Controller which it may ignore. This
 * parameter shall be ignored when there is only one BIS in the BIG.
 *
 * The Framing parameter indicates the format of the BIS Data PDUs and the mode of
 * framed PDUs that the Host is requesting the Controller to use on the specified BIG. The
 * Controller may use any combination of format and mode permitted by [Vol 6] Part G,
 * Table 2.1
 *
 * The Encryption parameter identifies the encryption mode of the BISes. If the Encryption
 * parameter is set to 1 (encrypted), then the Broadcast_Code is used in the encryption of
 * payloads (see [Vol 6] Part B, Section 4.4.6.10).
 *
 * The Broadcast_Code parameter is used to generate the encryption key for encrypting
 * payloads of all BISes. When the Encryption parameter is set to 0 (unencrypted),
 * the Broadcast_Code parameter shall be set to zero by the Host and ignored by the
 * Controller.
 *
 * If the Controller cannot create all BISes of the BIG or if Num_BIS exceeds the maximum
 * value supported by the Controller, then it shall return the error code Rejected due to
 * Limited Resources (0x0D).
 *
 * If the Advertising_Handle does not identify a periodic advertising train, the periodic
 * advertising train is associated with another BIG, or the periodic advertising train has
 * responses and the Controller does not support PAwR trains associated with BIGs (see
 * [Vol 6] Part B, Section 4.4.6.9), then the Controller shall return the error code Unknown
 * Advertising Identifier (0x42).
 *
 * If the Host issues this command with a BIG_Handle for a BIG that is already created,
 * then the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host specifies an invalid combination of BIG parameters, then the Controller shall
 * return an error which should use the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If the length of the associated periodic advertising, with the BIGInfo added to
 * the ACAD, is greater than the maximum that the Controller can transmit within
 * the periodic advertising interval (if periodic advertising is currently enabled) or the
 * Periodic_Advertising_Interval_Max for the advertising set (if currently disabled), then the
 * Controller shall return an error and should use the error code Packet Too Long (0x45).
 * If advertising on the LE Coded PHY, then the S=8 coding shall be assumed unless
 * the current advertising parameters require the use of S=2 for an advertising physical
 * channel, in which case the S=2 coding shall be assumed for that advertising physical
 * channel.
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Create_BIG command, the Controller sends
 * the HCI_Command_Status event to the Host. When the HCI_LE_Create_BIG command
 * has completed, the HCI_LE_Create_BIG_Complete event is generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_create_big(const sdc_hci_cmd_le_create_big_t * p_params);

/** @brief LE Create BIG Test.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.104
 *
 * This command should only be used for testing purposes.
 *
 * The command is used to create one or more BISes of a BIG (see [Vol 6] Part B,
 * Section 4.4.6). All BISes in the BIG have the same values for all parameters.
 *
 * The BIG_Handle contains the identifier of the BIG. This parameter is allocated by the
 * Host and used by the Controller and the Host to identify a BIG.
 *
 * The Advertising_Handle identifies the associated periodic advertising train of the BIG.
 *
 * The Num_BIS parameter contains the total number of BISes in the BIG.
 *
 * The SDU_Interval parameter specifies the time interval of the periodic SDUs.
 *
 * The ISO_Interval parameter contains the time duration between two consecutive BIG
 * anchor points.
 *
 * The NSE (Number of SubEvents) parameter contains the total number of subevents
 * that are used to transmit BIS Data PDUs for each BIS in a BIG event. The NSE
 * parameter shall be greater than or equal to IRC × BN.
 * The Max_SDU parameter contains the maximum size, in octets, of an SDU. The
 * minimum value of the Max_SDU parameter in the ISO Transmit Test mode when the
 * Payload_Type = 1 or 2 shall be 4.
 *
 * The Max_PDU parameter contains the maximum size of every BIS Data PDU for every
 * BIS in the BIG.
 *
 * The PHY parameter is a bit field that indicates the PHY used for transmission of PDUs
 * of BISes in the BIG. The Host shall set only one bit in this parameter and the Controller
 * shall use the PHY set by the Host. If the Host sets, in the PHY parameter, a bit for
 * a PHY that the Controller does not support, including a bit that is reserved for future
 * use, the Controller shall return the error code Unsupported Feature or Parameter Value
 * (0x11).
 *
 * The Packing parameter indicates the method of arranging subevents of multiple
 * BISes. The subevents can be arranged in Sequential or Interleaved arrangement. This
 * parameter shall be ignored when there is only one BIS in the BIG.
 *
 * The Framing parameter indicates the format of the BIS Data PDUs and the mode
 * of framed PDUs (see [Vol 6] Part G, Section 2) that the Controller shall use on the
 * specified BIG.
 *
 * The BN (Burst Number) parameter contains the number of new payloads for each BIS
 * in a BIS event.
 *
 * The IRC (Immediate Repetition Count) parameter contains the number of times the
 * scheduled data packet is transmitted (see [Vol 6] Part B, Section 4.4.6). The IRC
 * parameter shall be an integer in the range 1 to (NSE ÷ BN).
 *
 * The PTO (Pre_Transmission_Offset) parameter contains the offset in number of
 * ISO_Intervals for pre transmissions of data packets (see [Vol 6] Part B, Section 4.4.6).
 *
 * The Encryption parameter identifies the encryption mode of the BISes in the BIG. If
 * the Encryption parameter is set to 1 (encrypted), the Broadcast_Code is used in the
 * encryption of payloads (see [Vol 6] Part B, Section 4.4.6).
 *
 * The Broadcast_Code parameter is a 16-octet field that is used to generate the session
 * key to encrypt payloads of all BISes in the BIG. When the Encryption parameter is set to
 * 0 (unencrypted), all 16 octets of the Broadcast_Code parameter shall be set to zero by
 * the Host and ignored by the Controller.
 *
 * If the Controller cannot create all BISes of the BIG or if Num_BIS exceeds the maximum
 * value supported by the Controller, it shall return the error code Rejected due to Limited
 * Resources (0x0D).
 * If the Advertising_Handle does not identify a periodic advertising train, the periodic
 * advertising train is associated with another BIG, or the periodic advertising train has
 * responses and the Controller does not support PAwR trains associated with BIGs (see
 * [Vol 6] Part B, Section 4.4.6.9), the Controller shall return the error code Unknown
 * Advertising Identifier (0x42).
 *
 * If the Host issues this command with a BIG_Handle for a BIG that is already created,
 * the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the value of the Max_PDU, NSE, BN, IRC or PTO parameters exceeds the values
 * supported by the Controller, the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * If the Host specifies an invalid combination of BIG parameters, the Controller shall
 * return an error. If the value of the NSE parameter is not an integer multiple of BN,
 * or NSE is less than (IRC × BN), or the parameters are not in the specified range,
 * these errors shall use the error code Unsupported Feature or Parameter Value (0x11).
 * The errors in all other circumstances should use the error code Invalid HCI Command
 * Parameters (0x12).
 *
 * If the length of the associated periodic advertising, with the BIGInfo added to the
 * ACAD, is greater than the maximum that the Controller can transmit within the periodic
 * advertising interval, then the Controller shall return an error and should use the error
 * code Packet Too Long (0x45). If advertising on the LE Coded PHY, then the S=8 coding
 * shall be assumed unless the current advertising parameters require the use of S=2 for
 * an advertising physical channel, in which case the S=2 coding shall be assumed for that
 * advertising physical channel.
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Create_BIG_Test command,
 * the Controller sends the HCI_Command_Status event to the Host.
 * When the HCI_LE_Create_BIG_Test command has completed, the
 * HCI_LE_Create_BIG_Complete event is generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_create_big_test(const sdc_hci_cmd_le_create_big_test_t * p_params);

/** @brief LE Terminate BIG.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.105
 *
 * This command is used to terminate a BIG identified by the BIG_Handle parameter.
 * The command also terminates the transmission of all BISes of the BIG, destroys the
 * associated connection handles of the BISes in the BIG and removes the data paths for
 * all BISes in the BIG.
 *
 * The Reason parameter is used to indicate the reason why the BIG is to be terminated.
 *
 * If the BIG_Handle does not identify a BIG, the Controller shall return the error code
 * Unknown Advertising Identifier (0x42).
 *
 * If the Controller is not the Isochronous Broadcaster for the BIG identified by
 * BIG_Handle, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Terminate_BIG command, the Controller
 * sends the HCI_Command_Status event to the Host.
 * When the HCI_LE_Terminate_BIG command has completed, the
 * HCI_LE_Terminate_BIG_Complete event will be generated.
 *
 * If the Host attempts to terminate a BIG while the process of establishment of the
 * BIG is in progress (i.e. HCI_LE_Create_BIG_Complete event has not been generated)
 * the process of establishment shall stop and the Controller shall generate the HCI_LE_-
 * Create_BIG_Complete event to the Host with the error code Operation Cancelled by
 * Host (0x44).
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_terminate_big(const sdc_hci_cmd_le_terminate_big_t * p_params);

/** @brief LE BIG Create Sync.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.106
 *
 * This command is used to synchronize to a BIG described in the periodic advertising
 * train specified by the Sync_Handle parameter.
 *
 * The BIG_Handle parameter is assigned by the Host to identify the synchronized BIG.
 *
 * The Encryption parameter indicates whether the Broadcast_Code parameter is valid.
 *
 * The Broadcast_Code parameter is a 16-octet field that is used to generate the session
 * key to encrypt or decrypt payloads of an encrypted BIS. Broadcast_Code shall be
 * ignored by the Controller if Encryption is set to 0x00.
 *
 * If Encryption is set to 0x00 for an encrypted BIG or is set to 0x01 for an unencrypted
 * BIG, then the Controller shall return the error Encryption Mode Not Acceptable (0x25).
 *
 * The MSE (Maximum Subevents) parameter is the maximum number of subevents that
 * a Controller should listen to for data payloads in each interval for a BIS. The Controller
 * may select any of the subevents to listen to.
 *
 * The BIG_Sync_Timeout parameter specifies the maximum permitted time between
 * successful receptions of BIS PDUs. If this time is exceeded, synchronization is lost.
 * When the Controller establishes synchronization and if the BIG_Sync_Timeout set by
 * the Host is less than 6 × ISO_Interval, the Controller shall set the timeout to 6 ×
 * ISO_Interval.
 *
 * The Num_BIS parameter contains the number of BISes specified in the BIS arrayed
 * parameter. The number of BISes requested may be less than the number of BISes in
 * the BIG.
 * The BIS arrayed parameter is a list of BIS_Numbers corresponding to BIS(es) in the
 * synchronized BIG. The list of BIS_Numbers shall be in ascending order and shall not
 * contain any duplicates.
 *
 * If the Sync_Handle does not exist, the Controller shall return the error code Unknown
 * Advertising Identifier (0x42).
 *
 * If the Host sends this command with a BIG_Handle that is already allocated, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the information describing the BIG does not specify a PHY supported by the
 * Controller or does not specify exactly one PHY, the Controller shall return the error
 * code Unsupported Feature or Parameter Value (0x11).
 *
 * If the Num_BIS parameter is greater than the total number of BISes in the BIG, the
 * Controller shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If MSE is greater than zero and less than the BN value for the BIS, then the Controller
 * should return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host sends this command when the Controller is in the process of synchronizing
 * to any BIG, i.e. the HCI_LE_BIG_Sync_Established event has not been generated, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Controller is unable to receive PDUs from the specified number of BISes in the
 * synchronized BIG, it shall return the error code Rejected Due To Limited Resources
 * (0x0D).
 *
 * If the Controller is already synchronized to the BIG specified by Sync_Handle, it shall
 * return an error which should use the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_BIG_Create_Sync command,
 * the Controller sends the HCI_Command_Status event to the Host.
 * When the HCI_LE_BIG_Create_Sync command has completed, the
 * HCI_LE_BIG_Sync_Established event will be generated.
 *
 * If the Controller does not receive a BIS PDU within 6 BIS events of first listening, then it
 * shall generate an HCI_LE_BIG_Sync_Established event that should have Status set to
 * Connection Failed to be Established / Synchronization Timeout (0x3E).
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_big_create_sync(const sdc_hci_cmd_le_big_create_sync_t * p_params);

/** @brief LE BIG Terminate Sync.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.107
 *
 * This command is used to stop synchronizing or cancel the process of synchronizing
 * to the BIG identified by the BIG_Handle parameter. The command also terminates the
 * reception of BISes in the BIG specified in the HCI_LE_BIG_Create_Sync command,
 * destroys the associated connection handles of the BISes in the BIG and removes the
 * data paths for all BISes in the BIG.
 *
 * If the Host issues this command with a BIG_Handle that does not exist, the Controller
 * shall return the error code Unknown Advertising Identifier (0x42).
 *
 * If the Host issues this command for a BIG which it is neither synchronized to nor in the
 * process of synchronizing to, then the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_BIG_Terminate_Sync command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * If the Host attempts to terminate synchronization with a BIG while the process of
 * synchronization with that BIG is in progress (i.e. HCI_LE_BIG_Sync_Established event
 * has not been generated) the process of synchronization shall stop, and the Controller
 * shall generate the HCI_LE_BIG_Sync_Established event to the Host with the error code
 * Operation Cancelled by Host (0x44).
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_big_terminate_sync(const sdc_hci_cmd_le_big_terminate_sync_t * p_params,
                                          sdc_hci_cmd_le_big_terminate_sync_return_t * p_return);

/** @brief LE Request Peer SCA.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.108
 *
 * This command is used to read the Sleep Clock Accuracy (SCA) of the peer device.
 *
 * The Connection_Handle parameter is the connection handle of the ACL connection.
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL the Controller shall return the error code Unknown
 * Connection Identifier (0x02).
 *
 * If the Host sends this command and the peer device does not support the Sleep
 * Clock Accuracy Updates feature, the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11) in the HCI_LE_Request_Peer_SCA_Complete
 * event.
 *
 * If the Host issues this command when the Controller is aware (e.g., through a previous
 * feature exchange) that the peer device's Link Layer does not support the Sleep Clock
 * Accuracy Updates feature, the Controller shall return the error code Unsupported
 * Remote Feature (0x1A).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Request_Peer_SCA command,
 * the Controller sends the HCI_Command_Status event to the Host.
 * When the HCI_LE_Request_Peer_SCA command has completed, the
 * HCI_LE_Request_Peer_SCA_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_request_peer_sca(const sdc_hci_cmd_le_request_peer_sca_t * p_params);

/** @brief LE Setup ISO Data Path.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.109
 *
 * This command is used to identify and create the isochronous data path between
 * the Host and the Controller for a CIS, CIS configuration, or BIS identified by the
 * Connection_Handle parameter. This command can also be used to configure a codec
 * for each data path. When a connection is created no data paths are set up for that
 * connection. When the command has completed successfully, data shall be allowed to
 * flow over the specified path in the specified direction irrespective of the state of the
 * other direction or any other path.
 *
 * The input and output directions are defined from the perspective of the Controller, so
 * "input" refers to data flowing from the Host to the Controller.
 *
 * If the Host issues this command more than once for the same Connection_Handle
 * and direction before issuing the HCI_LE_Remove_ISO_Data_Path command for that
 * Connection_Handle and direction, the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * If the Host issues this command for a CIS on a Peripheral before it has issued the
 * HCI_LE_Accept_CIS_Request command for that CIS, then the Controller shall return
 * the error code Command Disallowed (0x0C).
 *
 * The Data_Path_Direction parameter specifies the direction for which the data path is
 * being configured.
 *
 * The Data_Path_ID parameter specifies the data transport path used. When set to 0x00,
 * the data path shall be over the HCI transport. When set to a value in the range
 * 0x01 to 0xFE, the data path shall use a vendor-specific transport interface (e.g., a
 * PCM interface) with logical transport numbers. The meanings of these logical transport
 * numbers are vendor-specific.
 * If the Host issues this command for a vendor-specific data transport path that has
 * not been configured, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * If the Host attempts to set a data path with a Connection Handle that does not exist or
 * that is not for a CIS, CIS configuration, or BIS, the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * If the Host attempts to set an output data path using a connection handle that is for an
 * Isochronous Broadcaster, for an input data path on a Synchronized Receiver, or for a
 * data path for the direction on a unidirectional CIS where BN is set to 0, the Controller
 * shall return the error code Command Disallowed (0x0C).
 *
 * The Codec_ID parameter specifies the coding format used over the air.
 *
 * When Data_Path_Direction is set to 0x00 (input), the Controller_Delay parameter
 * specifies the delay at the data source from the reference time of an SDU
 * to the CIG reference point (see [Vol 6] Part B, Section 4.5.14.1) or BIG
 * anchor point (see [Vol 6] Part B, Section 4.4.6.4). When Data_Path_Direction
 * is set to 0x01 (output), Controller_Delay specifies the delay from the
 * SDU_Synchronization_Reference to the point in time at which the Controller begins
 * to transfer the corresponding data to the data path interface. The Host should use the
 * HCI_Read_Local_Supported_Controller_Delay command to obtain a suitable value for
 * Controller_Delay.
 *
 * Note: Controller vendors may provide additional guidance to the Host on how to
 * select a suitable Controller_Delay value from the range of values provided by the
 * HCI_Read_Local_Supported_Controller_Delay command for various configurations of
 * the data path interface.
 *
 * The Codec_Configuration parameter specifies codec-specific configuration information
 * for the specified direction.
 *
 * If the Host issues this command with Codec_Configuration_Length non-zero and
 * Codec_ID set to transparent air mode, the Controller shall return the error code Invalid
 * HCI Command Parameters (0x12).
 *
 * If the Host issues this command with codec-related parameters that exceed the
 * bandwidth and latency allowed on the established CIS or BIS identified by the
 * Connection_Handle parameter, the Controller shall return the error code Invalid HCI
 * Command Parameters (0x12).
 *
 * If the Host issues this command when the CIS or BIS identified by Connection_Handle
 * is in ISO Test mode (see Section 7.8.111, Section 7.8.112, and [Vol 6] Part B, Section 7)
 * for the specified Data_Path_Direction, then the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Setup_ISO_Data_Path command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_setup_iso_data_path(const sdc_hci_cmd_le_setup_iso_data_path_t * p_params,
                                           sdc_hci_cmd_le_setup_iso_data_path_return_t * p_return);

/** @brief LE Remove ISO Data Path.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.110
 *
 * This command is used to remove the input and/or output data path(s) associated with a
 * CIS, CIS configuration, or BIS identified by the Connection_Handle parameter.
 *
 * The Data_Path_Direction parameter specifies which directions are to have the data
 * path removed.
 *
 * If the Host issues this command with a Connection_Handle that does not exist or is not
 * for a CIS, CIS configuration, or BIS, the Controller shall return the error code Unknown
 * Connection Identifier (0x02).
 *
 * If the Host issues this command for a data path that has not been set up (using the
 * HCI_LE_Setup_ISO_Data_Path command), the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Remove_ISO_Data_Path command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_remove_iso_data_path(const sdc_hci_cmd_le_remove_iso_data_path_t * p_params,
                                            sdc_hci_cmd_le_remove_iso_data_path_return_t * p_return);

/** @brief LE ISO Transmit Test.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.111
 *
 * This command should only be used in the ISO Test mode and only for testing purposes.
 *
 * The command is used to configure an established CIS or BIS specified by the
 * Connection_Handle parameter, and transmit test payloads which are generated by the
 * Controller.
 *
 * The Payload_Type parameter defines the configuration of SDUs in the payload.
 *
 * If the Host issues this command with a connection handle that does not exist, or the
 * Connection_Handle parameter is not associated with a CIS or a BIS, then the Controller
 * shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the Host issues this command when the value of the transmit BN parameter of the
 * CIS is set to zero, the Controller shall return the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * If the Host has set the input data path for the CIS or BIS identified by the connection
 * handle, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_ISO_Transmit_Test command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_iso_transmit_test(const sdc_hci_cmd_le_iso_transmit_test_t * p_params,
                                         sdc_hci_cmd_le_iso_transmit_test_return_t * p_return);

/** @brief LE ISO Receive Test.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.112
 *
 * This command should only be used in the ISO Test mode and only for testing purposes.
 *
 * The command is used to configure an established CIS or a synchronized BIG specified
 * by the Connection_Handle parameter to receive payloads.
 *
 * When using this command for a BIS, the Host shall synchronize with a BIG using the
 * HCI_LE_BIG_Create_Sync command before invoking this command.
 *
 * The Payload_Type parameter defines the configuration of SDUs in the payload.
 *
 * If the Host issues this command with a connection handle that is not for an established
 * CIS or a BIS, the Controller shall return the error code Unknown Connection Identifier
 * (0x02).
 *
 * If the Host issues this command when the value of the receive BN parameter of the CIS
 * or BIS is set to zero, the Controller shall return the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * If the Host has set the output data path for the CIS or BIS identified by the
 * Connection_Handle parameter, the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_ISO_Receive_Test command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_iso_receive_test(const sdc_hci_cmd_le_iso_receive_test_t * p_params,
                                        sdc_hci_cmd_le_iso_receive_test_return_t * p_return);

/** @brief LE ISO Read Test Counters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.113
 *
 * This command should only be used in the ISO Test mode and only for testing purposes.
 *
 * The command is used to read the test counters (see [Vol 6] Part B, Section 7) in the
 * Controller which is configured in ISO Receive Test mode for a CIS or BIS specified by
 * the Connection_Handle. Reading the test counters does not reset the test counters.
 *
 * The Received_SDU_Count, Missed_SDU_Count and Failed_SDU_Count parameters
 * are set in the ISO Receive Test mode (see [Vol 6] Part B, Section 7.2).
 *
 * If the Host issues this command with a Connection_Handle parameter that is not for an
 * established CIS or a BIS, the Controller shall return the error code Unknown Connection
 * Identifier (0x02).
 *
 * If the Host issues this command for a CIS or BIS that is not configured in the ISO
 * Receive Test mode, the Controller shall return the error code Unsupported Feature or
 * Parameter Value (0x11).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_ISO_Read_Test_Counters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_iso_read_test_counters(const sdc_hci_cmd_le_iso_read_test_counters_t * p_params,
                                              sdc_hci_cmd_le_iso_read_test_counters_return_t * p_return);

/** @brief LE ISO Test End.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.114
 *
 * This command should only be used in the ISO Test mode and only for testing purposes.
 *
 * The command is used to terminate the ISO Transmit and/or Receive Test mode for a
 * CIS or BIS specified by the Connection_Handle parameter but does not terminate the
 * CIS or BIS.
 *
 * When the Host terminates the ISO Test mode for a CIS or BIS that is set to ISO
 * Transmit Test mode only, the test counters in the return parameters shall be set to zero.
 *
 * When the Host terminates the ISO Test mode for a CIS or BIS that is set to the ISO
 * Receive Test mode, the return parameters contain the values of the test counters as
 * defined in [Vol 6] Part B, Section 7.
 *
 * If the Host issues this command with a Connection_Handle that is not for an established
 * CIS or a BIS, the Controller shall return the error code Unknown Connection Identifier
 * (0x02).
 *
 * If the Host issues this command for a CIS or BIS that is not configured in the ISO
 * Transmit or Receive Test mode, the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_ISO_Test_End command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_iso_test_end(const sdc_hci_cmd_le_iso_test_end_t * p_params,
                                    sdc_hci_cmd_le_iso_test_end_return_t * p_return);

/** @brief LE Set Host Feature [v1].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.115
 *
 * This command is used by the Host to set or clear a bit controlled by the Host in the Link
 * Layer FeatureSet stored in the Controller (see [Vol 6] Part B, Section 4.6).
 *
 * The Bit_Number parameter specifies the bit position in the FeatureSet.
 *
 * The Bit_Value parameter specifies whether the feature is enabled or disabled.
 *
 * If Bit_Number specifies a feature bit that is not controlled by the Host, the Controller
 * shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If Bit_Value is set to 0x01 and Bit_Number specifies a feature bit that requires support of
 * a feature that the Controller does not support, the Controller shall return the error code
 * Unsupported Feature or Parameter Value (0x11).
 *
 * If the Host issues this command while the Controller has a connection to another
 * device, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Host_Feature command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_host_feature(const sdc_hci_cmd_le_set_host_feature_t * p_params);

/** @brief LE Read ISO Link Quality.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.116
 *
 * This command returns the values of various counters related to link quality that are
 * associated with the isochronous stream specified by the Connection_Handle parameter.
 *
 * This command may be issued on both the Central and Peripheral if the connection
 * handle identifies a CIS and on the Synchronized Receiver if the connection handle
 * identifies a BIS.
 *
 * Each of the remaining return parameters shall contain the current value of the
 * corresponding counter; all the values shall be recorded at the same moment. Each
 * counter shall be a 32-bit unsigned value, shall be initialized to zero when the
 * isochronous stream is created, and shall be incremented by one as described below. If
 * a counter is not associated with the type of isochronous stream specified, the value of
 * the parameter shall be ignored.
 *
 *  Counter                 Associated         When Incremented
 *                          Streams
 *  TX_UnACKed_Packets      CIS                The Link Layer does not receive an acknowledgment for
 *                                             a CIS Data PDU that it transmitted at least once by
 * its
 *                                             flush point (see [Vol 6] Part B, Section 4.5.13.5).
 *  TX_Flushed_Packets      CIS                The Link Layer does not transmit a specific payload
 * by
 *                                             its flush point.
 *  TX_Last_Subevent_-      CIS in Peripher-   The Link Layer transmits a CIS Data PDU in the last
 *  Packets                 al role            subevent of a CIS event.
 *  Retransmitted_Packets   CIS                The Link Layer retransmits a CIS Data PDU.
 *  CRC_Error_Packets       CIS and BIS        The Link Layer receives a packet with a CRC error.
 *  Counter                    Associated          When Incremented
 *                             Streams
 *  RX_Unreceived_-            CIS and BIS         The Link Layer does not receive a specific
 * payload by
 *  Packets                                        its flush point (on a CIS) or the end of the
 * event it is
 *                                                 associated with (on a BIS; see [Vol 6] Part B,
 * Section
 *                                                 4.4.6.6).
 *  Duplicate_Packets          CIS                 The Link Layer receives a retransmission of a CIS
 * Data
 *                                                 PDU.
 *
 * Table 7.4: Isochronous streams link quality counters
 *
 * If the Connection_Handle parameter does not identify a current CIS connection or a BIS
 * that the Controller is synchronized to, the Controller shall return the error code Unknown
 * Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_ISO_Link_Quality command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_iso_link_quality(const sdc_hci_cmd_le_read_iso_link_quality_t * p_params,
                                             sdc_hci_cmd_le_read_iso_link_quality_return_t * p_return);

/** @brief LE Enhanced Read Transmit Power Level.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.117
 *
 * This command is used to read the current and maximum transmit power levels of the
 * local Controller on the ACL connection identified by the Connection_Handle parameter
 * and the PHY indicated by the PHY parameter.
 *
 * If the Host sets PHY to a value that the Controller does not support, including a value
 * that is reserved for future use, the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * If the Connection_Handle parameter does not identify a current ACL connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Enhanced_Read_Transmit_Power_Level command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_enhanced_read_transmit_power_level(const sdc_hci_cmd_le_enhanced_read_transmit_power_level_t * p_params,
                                                          sdc_hci_cmd_le_enhanced_read_transmit_power_level_return_t * p_return);

/** @brief LE Read Remote Transmit Power Level.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.118
 *
 * This command is used to read the transmit power level used by the remote Controller
 * on the ACL connection that is identified by the Connection_Handle parameter and the
 * PHY indicated by the PHY parameter.
 *
 * The local Controller may use the remote transmit power level value obtained from a
 * prior Power Change Indication or Power Control Request procedure (see [Vol 6] Part B,
 * Section 5.1.17 and [Vol 6] Part B, Section 5.1.18). If the Controller chooses not to use
 * these prior values, or if no prior value is available for one or more of the remote transmit
 * power level, maximum transmit power level, or minimum transmit power level, the local
 * Controller shall initiate a new Power Control Request procedure to obtain the remote
 * transmit power level.
 *
 * If the Host sets PHY to a value that the Controller does not support, including a value
 * that is reserved for future use, the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * If the Connection_Handle parameter does not identify a current ACL connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Read_Remote_Transmit_Power_Level
 * command, the Controller shall send the HCI_Command_Status event to the Host.
 * When the Controller has determined the remote transmit power, it shall generate an
 * HCI_LE_Transmit_Power_Reporting event with Reason 0x02.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_remote_transmit_power_level(const sdc_hci_cmd_le_read_remote_transmit_power_level_t * p_params);

/** @brief LE Set Path Loss Reporting Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.119
 *
 * This command is used to set the path loss threshold reporting parameters for the ACL
 * connection identified by the Connection_Handle parameter.
 *
 * The path loss threshold-based mechanism is described in [Vol 6] Part B, Section 4.5.16.
 * For each zone boundary, the upwards boundary shall equal the threshold plus the
 * hysteresis and the downwards boundary shall equal the threshold minus the hysteresis.
 *
 * If the Host issues this command with High_Threshold+High_Hysteresis greater than
 * 0xFF or with Low_Threshold less than Low_Hysteresis, the Controller shall return the
 * error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command with Low_Threshold greater than High_Threshold
 * or with Low_Threshold+Low_Hysteresis greater than High_Threshold–High_Hysteresis,
 * the Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * The Min_Time_Spent parameter indicates the minimum time that the Controller shall
 * observe the path loss has crossed the threshold before the Controller generates an
 * event for the threshold crossing. The Host should specify a suitable value based on the
 * connection interval, subrate factor, and Peripheral latency.
 *
 * If the Host issues this command when path loss monitoring is enabled, the Controller
 * shall override the existing path loss threshold reporting parameters with the parameters
 * provided in this command.
 *
 * The High_Threshold and the Low_Threshold parameters are common to all PHYs
 * supported by the Controller. However, the Host can reissue this command with suitable
 * parameters whenever a PHY switch is detected.
 *
 * If the Connection_Handle parameter does not identify a current ACL connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Path_Loss_Reporting_Parameters command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * If the Host issues this command when path loss monitoring is enabled, and
 * if the new parameters mean that the path loss is now in a different zone,
 * an HCI_LE_Path_Loss_Threshold event shall be generated as soon as possible
 * irrespective of the Min_Time_Spent parameter and the timer shall be reset.
 *
 * If the Host issues this command with High_Threshold parameter set to 0xFF,
 * then the Controller shall not generate an HCI_LE_Path_Loss_Threshold event with
 * Zone_Entered set to 0x02.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_path_loss_reporting_params(const sdc_hci_cmd_le_set_path_loss_reporting_params_t * p_params,
                                                      sdc_hci_cmd_le_set_path_loss_reporting_params_return_t * p_return);

/** @brief LE Set Path Loss Reporting Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.120
 *
 * This command is used to enable or disable path loss reporting for the ACL connection
 * identified by the Connection_Handle parameter.
 *
 * If the Enable parameter is set to 0x01 and no prior LE Power Control Request
 * procedure has been initiated on the ACL connection, then the Controller may need
 * to initiate a new LE Power Control Request procedure on that ACL.
 *
 * Path loss reporting is disabled when the connection is first created.
 *
 * If the Host issues this command before it has issued the
 * HCI_LE_Set_Path_Loss_Reporting_Parameters command on this connection, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Connection_Handle parameter does not identify a current ACL connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * Enabling path loss monitoring when it is already enabled or disabling path loss
 * monitoring when it is already disabled has no effect.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Path_Loss_Reporting_Enable command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * When reporting is enabled and was previously disabled, the Controller shall generate
 * an HCI_LE_Path_Loss_Threshold event as soon as it has a reliable measurement
 * of the path loss. If the Controller has to query the remote Controller for its transmit
 * power level, then it shall generate this event within Tpath_loss_enable from the time it
 * receives a response to its query. Otherwise, the Controller shall generate this event
 * within Tpath_loss_enable from the time the command is issued. Tpath_loss_enable shall be
 * Min_Time_Spent + 6 connection events or, if longer, 2 connection events where the
 * Controller actually receives a packet from the peer, where Min_Time_Spent is specified
 * by the HCI_LE_Set_Path_Loss_Reporting_Parameters command.
 *
 * After the initial event on reporting being enabled, the Controller shall generate this
 * event each time it determines that the path loss has moved to a different zone and
 * stayed in that zone for Min_Time_Spent.As stated in [Vol 6] Part B, Section 4.5.16, two
 * consecutive events must not indicate the same zone.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_path_loss_reporting_enable(const sdc_hci_cmd_le_set_path_loss_reporting_enable_t * p_params,
                                                      sdc_hci_cmd_le_set_path_loss_reporting_enable_return_t * p_return);

/** @brief LE Set Transmit Power Reporting Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.121
 *
 * This command is used to enable or disable the reporting to the local Host of transmit
 * power level changes in the local and remote Controllers for the ACL connection
 * identified by the Connection_Handle parameter.
 *
 * If the Remote_Enable parameter is set to 0x01 and no prior LE Power Control Request
 * procedure has been initiated on the ACL connection, then the Controller shall initiate a
 * new LE Power Control Request procedure on that ACL.
 *
 * Reporting is disabled when the connection is first created.
 *
 * If the Connection_Handle parameter does not identify a current ACL connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Transmit_Power_Reporting_Enable command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * When local reporting is enabled, the Controller shall generate an
 * HCI_LE_Transmit_Power_Reporting event with Reason 0x00 each time the local
 * transmit power level is changed.
 *
 * When remote reporting is enabled, the Controller shall generate an
 * HCI_LE_Transmit_Power_Reporting event with Reason 0x01 each time it becomes
 * aware that the remote transmit power level has changed.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_transmit_power_reporting_enable(const sdc_hci_cmd_le_set_transmit_power_reporting_enable_t * p_params,
                                                           sdc_hci_cmd_le_set_transmit_power_reporting_enable_return_t * p_return);

/** @brief LE Transmitter Test [v4].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.29
 *
 * This command is used to start a test where the IUT generates test reference packets
 * at a fixed interval. The Controller shall transmit at the power level indicated by the
 * TX_Power_Level parameter.
 *
 * The TX_Channel and PHY parameters specify the RF channel and PHY to be used by
 * the transmitter. If the Host sets the PHY parameter to a PHY that the Controller does
 * not support, including a value that is reserved for future use, the Controller shall return
 * the error code Unsupported Feature or Parameter Value (0x11).
 *
 * The Test_Data_Length and Packet_Payload parameters specify the length and
 * contents of the Payload of the test reference packets. An LE Controller supporting
 * the HCI_LE_Transmitter_Test command shall support Packet_Payload values 0x00,
 * 0x01 and 0x02. An LE Controller supporting the LE Coded PHY shall also
 * support Packet_Payload value 0x04. An LE Controller may support other values of
 * Packet_Payload.
 *
 * The CTE_Length and CTE_Type parameters specify the length and type of the
 * Constant Tone Extension in the test reference packets. If the CTE_Type parameter
 * is set to 0x01 and the Controller does not support 1 µs switching, the Controller shall
 * return the error code Unsupported Feature or Parameter Value (0x11). If CTE_Length is
 * not zero and PHY specifies a PHY that does not allow Constant Tone Extensions, the
 * Controller shall return the error code Command Disallowed (0x0C).
 *
 * The Switching_Pattern_Length and Antenna_IDs[i] parameters specify the antenna
 * switching pattern. They are only used when transmitting an AoD Constant Tone
 * Extension and shall be ignored when transmitting an AoA Constant Tone Extension.
 *
 * If the Controller determines that any of the Antenna_IDs[i] values do not identify an
 * antenna in the device's antenna array, it shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * Note: Some Controllers may be unable to determine which values do or do not identify
 * an antenna.
 *
 * The TX_Power_Level parameter specifies the transmit power level to be used by
 * the transmitter. If the parameter is set to a value other than 0x7E or 0x7F, then the
 * Controller shall make the requested change or shall make the nearest change that it is
 * capable of doing.
 *
 * Event(s) generated (unless masked away):
 * When the HCI_LE_Transmitter_Test command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_transmitter_test_v4(const sdc_hci_cmd_le_transmitter_test_v4_t * p_params);

/** @brief LE Set Data Related Address Changes.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.122
 *
 * This command specifies circumstances when the Controller shall refresh any
 * Resolvable Private Address used by the advertising set identified by the
 * Advertising_Handle parameter, whether or not the address timeout period has been
 * reached. This command may be used while advertising is enabled.
 *
 * The Change_Reasons parameter specifies the reason(s) for refreshing addresses. The
 * default when an advertising set is created, or if legacy advertising commands (see
 * Section 3.1.1) are used, is for all bits to be clear.
 *
 * If extended advertising commands (see Section 3.1.1) are being used and the
 * advertising set corresponding to the Advertising_Handle parameter does not exist, or
 * if no command specified in Table 3.2 has been used, then the Controller shall return the
 * error code Unknown Advertising Identifier (0x42).
 *
 * If legacy advertising commands are being used, the Controller shall ignore the
 * Advertising_Handle parameter.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Data_Related_Address_Changes command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_data_related_address_changes(const sdc_hci_cmd_le_set_data_related_address_changes_t * p_params);

/** @brief LE Set Default Subrate.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.123
 *
 * This command is used by the Host to set the initial values for the acceptable
 * parameters for subrating requests, as defined by the HCI_LE Subrate_Request
 * command (see Section 7.8.124), for all future ACL connections where the Controller
 * is the Central. This command does not affect any existing connection.
 *
 * The parameters have the same meanings as those in the HCI_LE_Subrate_Request
 * command.
 *
 * If the Host issues this command with Subrate_Max × (Max_Latency + 1) greater than
 * 500, then the Controller shall return the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If the Host issues this command with Subrate_Max less than Subrate_Min, then the
 * Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command with Continuation_Number greater than or equal to
 * Subrate_Max, then the Controller shall return the error code Invalid HCI Command
 * Parameters (0x12).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Set_Default_Subrate command, the
 * Controller sends the HCI_Command_Complete event to the Host.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_default_subrate(const sdc_hci_cmd_le_set_default_subrate_t * p_params);

/** @brief LE Subrate Request.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.124
 *
 * This command is used by a Central or a Peripheral to request a change to the subrating
 * factor and/or other parameters (see [Vol 6] Part B, Section 4.5.1) applied to an existing
 * connection using the Connection Subrate Update procedure.
 *
 * The Subrate_Min and Subrate_Max parameters specify the range of acceptable
 * subrating factors being requested.
 *
 * The Max_Latency parameter specifies the maximum Peripheral latency in units of
 * subrated connection events. The same maximum shall apply irrespective of the
 * subrating factor actually chosen.
 *
 * The Continuation_Number parameter specifies the number of underlying connection
 * intervals to remain active after a packet (other than an empty packet) is transmitted or
 * received.
 *
 * The Supervision_Timeout parameter specifies the link supervision timeout for the
 * connection. The Supervision_Timeout, in milliseconds, shall be greater than 2 × current
 * connection interval × Subrate_Max × (Max_Latency + 1).
 *
 * If this command is issued on the Central, the following rules shall apply when the
 * Controller initiates the Connection Subrate Update procedure (see [Vol 6] Part B,
 * Section 5.1.19):
 *
 * • The Peripheral latency shall be less than or equal to Max_Latency.
 * • The subrate factor shall be between Subrate_Min and Subrate_Max.
 * • The continuation number shall be equal to the lesser of Continuation_Number and
 *   (subrate factor - 1).
 * • The connection supervision timeout shall be equal to Supervision_Timeout.
 *
 * If this command is issued on the Central, it also sets the acceptable parameters
 * for requests from the Peripheral (see [Vol 6] Part B, Section 5.1.20). The
 * acceptable parameters set by this command override those provided via the
 * HCI_LE_Set_Default_Subrate command or any values set by previous uses of this
 * command on the same connection.
 *
 * If this command is issued on the Central before the devices have performed the Feature
 * Exchange procedure, then the Controller shall complete that procedure before initiating
 * the Connection Subrate Update procedure.
 *
 * If this command is issued on the Peripheral, the following rules shall apply when the
 * Controller initiates the Connection Subrate Request procedure:
 *
 * • The Peripheral latency shall be less than or equal to Max_Latency.
 * • The minimum and maximum subrate factors shall be between Subrate_Min and
 *   Subrate_Max.
 * • The continuation number shall be equal to the lesser of Continuation_Number and
 *   (maximum subrate factor - 1).
 * • The connection supervision timeout shall be equal to Supervision_Timeout.
 *
 * If the Connection_Handle parameter does not identify a current ACL connection, the
 * Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the Host issues this command with parameters such that Subrate_Max ×
 * (Max_Latency + 1) is greater than 500 or the current connection interval × Subrate_Max
 * × (Max_Latency + 1) is greater than or equal to half the Supervision_Timeout
 * parameter, the Controller shall return the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If the Host issues this command with Subrate_Max less than Subrate_Min, the
 * Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command with Continuation_Number greater than or equal to
 * Subrate_Max, then the Controller shall return the error code Invalid HCI Command
 * Parameters (0x12).
 *
 * If the Central's Host issues this command when the Connection Subrating (Host
 * Support) bit is not set in the Peripheral's FeatureSet, the Controller shall return the
 * error code Unsupported Remote Feature (0x1A).
 *
 * If a Connection Rate Request procedure or a Connection Rate Update procedure is in
 * progress, then the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Subrate_Request command, the Controller
 * sends the HCI_Command_Status event to the Host. An HCI_LE_Subrate_Change
 * event shall be generated when the Connection Subrate Update procedure has
 * completed.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_subrate_request(const sdc_hci_cmd_le_subrate_request_t * p_params);

/** @brief LE Set Extended Advertising Parameters [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.53
 *
 * This command is used by the Host to set the advertising parameters.
 *
 * The Advertising_Handle parameter identifies the advertising set whose parameters are
 * being configured.
 *
 * The Advertising_Event_Properties parameter describes the type of advertising event
 * that is being configured and its basic properties. The type shall be one supported by the
 * Controller. In particular, the following restrictions apply to this parameter:
 *
 * • If legacy advertising PDU types are being used, then the parameter value shall be
 *   one of those specified in Table 7.3. If the advertising set already contains data, the
 *   type shall be one that supports advertising data and the amount of data shall not
 *   exceed 31 octets.
 *
 *  Event Type                    PDU Type               Advertising Event     Advertising Data
 *                                                       Properties
 *  Connectable and scannable     ADV_IND                0b00000000_00010011   Supported
 *  undirected
 *  Connectable directed          ADV_DIRECT_IND         0b00000000_00010101   Not allowed
 *  (low duty cycle)
 *  Connectable directed          ADV_DIRECT_IND         0b00000000_00011101   Not allowed
 *  (high duty cycle)
 *  Scannable undirected          ADV_SCAN_IND           0b00000000_00010010   Supported
 *  Non-connectable and non-      ADV_NON-               0b00000000_00010000   Supported
 *  scannable undirected          CONN_IND
 *
 * Table 7.3: Advertising_Event_Properties values for legacy PDUs
 *
 * • If extended advertising PDU types are being used (bit 4 = 0), then the advertisement
 *   shall not be both connectable and scannable (bits 0 and 1 must not both be set to 1)
 *   and high duty cycle directed connectable advertising (≤ 3.75 ms advertising interval)
 *   shall not be used (bit 3 = 0).
 *
 * If the Advertising_Event_Properties parameter does not describe an event type
 * supported by the Controller, contains an invalid bit combination, or specifies a type
 * that does not support advertising data when the advertising set already contains some,
 * the Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * The parameters beginning with “Secondary” are only valid when extended advertising
 * PDU types are being used (bit 4 = 0).
 *
 * The Own_Address_Type parameter shall be ignored for undirected anonymous
 * advertising (bit 2 = 0 and bit 5 = 1).
 * If Directed advertising is selected, the Peer_Address_Type and Peer_Address shall be
 * valid and the Advertising_Filter_Policy parameter shall be ignored.
 *
 * The Primary_Advertising_Interval_Min parameter shall be less than or equal to the
 * Primary_Advertising_Interval_Max parameter. The Primary_Advertising_Interval_Min
 * and Primary_Advertising_Interval_Max parameters should not be the same value so
 * that the Controller can choose the best advertising interval given other activities.
 *
 * For high duty cycle connectable directed advertising event type (ADV_DIRECT_IND),
 * the Primary_Advertising_Interval_Min and Primary_Advertising_Interval_Max
 * parameters are not used and shall be ignored.
 *
 * If the primary advertising interval range provided by the Host
 * (Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max) does not
 * overlap with the advertising interval range supported by the Controller, then the
 * Controller shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * The Primary_Advertising_Channel_Map is a bit field that indicates the advertising
 * channel indices that shall be used when transmitting advertising packets. At least one
 * channel bit shall be set in the Primary_Advertising_Channel_Map parameter.
 *
 * The Own_Address_Type parameter specifies the type of address being used in
 * the advertising packets. For random addresses, the address is specified by the
 * HCI_LE_Set_Advertising_Set_Random_Address command.
 *
 * If Own_Address_Type equals 0x02 or 0x03, the Peer_Address parameter contains
 * the peer’s Identity Address and the Peer_Address_Type parameter contains the
 * peer’s Identity Type (i.e., 0x00 or 0x01). These parameters are used to locate the
 * corresponding local IRK in the resolving list; this IRK is used to generate their own
 * address used in the advertisement.
 *
 * The Advertising_TX_Power parameter indicates the maximum power level at which the
 * advertising packets are to be transmitted on the advertising physical channels. The
 * Controller shall choose a power level lower than or equal to the one specified by the
 * Host.
 *
 * The Primary_Advertising_PHY parameter indicates the PHY on which the advertising
 * packets are transmitted on the primary advertising physical channel. If legacy
 * advertising PDUs are being used, the Primary_Advertising_PHY shall indicate the LE
 * 1M PHY. The Secondary_Advertising_PHY parameter indicates the PHY on which the
 * advertising packets are be transmitted on the secondary advertising physical channel.
 * If the Host specifies a PHY that is not supported by the Controller, including a value
 * that is reserved for future use, it should return the error code Unsupported Feature or
 * Parameter Value (0x11). If Constant Tone Extensions are enabled for the advertising set
 * and Secondary_Advertising_PHY specifies a PHY that does not allow Constant Tone
 * Extensions, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Primary_Advertising_PHY indicates the LE Coded PHY, then the
 * Primary_Advertising_PHY_Options shall indicate the Host's preference or requirement
 * concerning coding scheme. Otherwise, Primary_Advertising_PHY_Options shall
 * be ignored. If the Secondary_Advertising_PHY indicates the LE Coded PHY,
 * then the Secondary_Advertising_PHY_Options shall indicate the Host's preference
 * or requirement concerning coding scheme (including for periodic advertising).
 * Otherwise, Secondary_Advertising_PHY_Options shall be ignored. If the Host
 * specifies that it requires a specific coding (i.e., value 0x03 or 0x04) in the
 * Primary_Advertising_PHY_Options or Secondary_Advertising_PHY_Options and the
 * Controller supports the LE Feature (Advertising Coding Selection) but is currently
 * unable to provide all the required settings, then the Controller shall return the error
 * code Command Disallowed (0x0C).
 *
 * The Secondary_Advertising_Max_Skip parameter is the maximum number of
 * advertising events that can be skipped before the AUX_ADV_IND can be sent.
 *
 * The Advertising_SID parameter specifies the value to be transmitted in the Advertising
 * SID subfield of the ADI field of the Extended Header of those advertising physical
 * channel PDUs that have an ADI field. If the advertising set only uses PDUs that do not
 * contain an ADI field, Advertising_SID shall be ignored.
 *
 * The Scan_Request_Notification_Enable parameter indicates whether the Controller
 * shall send notifications upon the receipt of a scan request PDU that is in response
 * to an advertisement from the specified advertising set that contains its device address
 * and is from a scanner that is allowed by the advertising filter policy.
 *
 * The Controller shall set the Selected_TX_Power parameter to the transmit power that
 * it will use for transmitting the advertising packets for the specified advertising set. The
 * Controller shall only change this value if requested by the Host. If the radiated power
 * level will vary between packets (e.g., because of frequency-dependent properties of the
 * transmitter) then the value should be the best estimate of the maximum power used.
 *
 * If the Host issues this command when advertising is enabled for the specified
 * advertising set, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host issues this command when periodic advertising is enabled for the specified
 * advertising set and connectable, scannable, legacy, or anonymous advertising is
 * specified, the Controller shall return the error code Invalid HCI Command Parameters
 * (0x12).
 *
 * If periodic advertising is enabled for the advertising set and the
 * Secondary_Advertising_PHY parameter does not specify the PHY currently being
 * used for the periodic advertising, the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * If the Advertising_Handle does not identify an existing advertising set and the Controller
 * is unable to support a new advertising set at present, the Controller shall return the error
 * code Memory Capacity Exceeded (0x07).
 *
 * If the advertising set already contains advertising data or scan response data, extended
 * advertising is being used, and the length of the data is greater than the maximum that
 * the Controller can transmit within the longest possible auxiliary advertising segment
 * consistent with the parameters, the Controller shall return the error code Packet Too
 * Long (0x45). If advertising on the LE Coded PHY, the S=8 coding shall be assumed
 * unless the current advertising parameters require the use of S=2 for an advertising
 * physical channel, in which case the S=2 coding shall be assumed for that advertising
 * physical channel.
 *
 * If the Controller does not support the LE Feature (Advertising Coding
 * Selection) and the Host does not set both Primary_Advertising_PHY_Options and
 * Secondary_Advertising_PHY_Options to zero, the Controller shall return the error code
 * Unsupported Feature or Parameter Value (0x11).
 *
 * Missing parameters:
 *
 * When a version of this command is issued that does not include all the parameters, the
 * following values shall be used for any missing parameters:
 *
 *  Parameter                                                                   Value
 *  Primary_Advertising_PHY_Options                                             0x00
 *  Secondary_Advertising_PHY_Options                                           0x00
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Extended_Advertising_Parameters command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_ext_adv_params_v2(const sdc_hci_cmd_le_set_ext_adv_params_v2_t * p_params,
                                             sdc_hci_cmd_le_set_ext_adv_params_v2_return_t * p_return);

/** @brief LE Set Periodic Advertising Subevent Data.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.125
 *
 * This command is used by the Host to set the data for one or more subevents of PAwR
 * in reply to an HCI_LE_Periodic_Advertising_Subevent_Data_Request event. The data
 * for a subevent shall be transmitted only once.
 *
 * When using more than one instance of this command to provide data, the Host may
 * provide the data in any order. For example, if providing data for subevents 0 and 1 in
 * separate commands, the first command can contain either subevent.
 *
 * The Advertising_Handle parameter identifies the advertising set whose periodic
 * advertising subevent data is being set. If the corresponding advertising set does not
 * already exist, then the Controller shall return the error code Unknown Advertising
 * Identifier (0x42). If the corresponding advertising set exists but has not been configured
 * for Periodic Advertising with Responses, then the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * The Num_Subevents_With_Data parameter is the number of subevent data contained
 * in the parameter arrays.
 *
 * The Subevent[i] parameter identifies the subevent of the PAwR that is being set.
 * If the Host provides a subevent value that is outside of the range of subevents
 * requested using the HCI_LE_Periodic_Advertising_Subevent_Data_Request event,
 * then the Controller shall return the error code Command Disallowed (0x0C). If two
 * elements of the array specify the same subevent, then the Controller shall return an
 * error code which should be Invalid HCI Command Parameters (0x12).
 *
 * The Response_Slot_Start[i] and Response_Slot_Count[i] parameters identify the
 * starting response slot and the number of response slots that are expected to be used in
 * this subevent.
 * Note: If the Host does not expect any responses in a subevent, then it can
 * set Response_Slot_Count[i] for that subevent to 0. If the Host expects one or
 * more responses in a future subevent, then it can set Response_Slot_Start[i] and
 * Response_Slot_Count[i] for that subevent appropriately either in the same command
 * or a subsequent use of this command.
 *
 * The Subevent_Data_Length[i] parameter determines the length of the Subevent_Data
 * that is significant.
 *
 * The Subevent_Data[i] parameter contains the advertising data to be transmitted in
 * the subevent of the advertising set. If the combined data length is greater than the
 * maximum that the Controller can transmit within the current subevent interval, then all
 * data shall be discarded and the Controller shall return the error code Packet Too Long
 * (0x45). If advertising on the LE Coded PHY, then the S=8 coding shall be assumed
 * unless the current advertising parameters require the use of S=2 for an advertising
 * physical channel, in which case the S=2 coding shall be assumed for that advertising
 * physical channel.
 *
 * If the Subevent_Data cannot be transmitted because, for example, the subevent where
 * this data would have been sent has already passed or is too early, then the Controller
 * shall return the error code Too Late (0x46) or Too Early (0x47) and discard the data.
 *
 * If the Host sends this command without the Controller having issued an
 * HCI_LE_Periodic_Advertising_Subevent_Data_Request event, or sends it twice for the
 * same subevent of the same periodic advertising event, then the Controller shall return
 * an error which should use the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Advertising_Subevent_Data command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_subevent_data(const sdc_hci_cmd_le_set_periodic_adv_subevent_data_t * p_params,
                                                      sdc_hci_cmd_le_set_periodic_adv_subevent_data_return_t * p_return);

/** @brief LE Set Periodic Advertising Response Data.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.126
 *
 * This command is used by the Host to set the data for a response slot in a specific
 * subevent of the PAwR identified by the Sync_Handle. The data for a response slot shall
 * be transmitted only once.
 *
 * The Request_Event parameter identifies the periodic advertising event in which the
 * periodic advertising packet that the Host is responding to was received.
 *
 * The Request_Subevent parameter identifies the subevent in which the periodic
 * advertising packet that the Host is responding to was received.
 *
 * The Response_Subevent parameter identifies the subevent that the response shall be
 * sent in.
 *
 * The Response_Slot parameter identifies the response slot in the subevent identified by
 * the Response_Subevent parameter in which this response data is to be transmitted.
 *
 * The Response_Data_Length specifies the length of the Response_Data that is
 * significant.
 *
 * The Response_Data contains the advertising data to be transmitted in the response
 * slot. If the Response_Data_Length is greater than the maximum that the Controller can
 * transmit within the response slot, then the Response_Data shall be discarded and the
 * Controller shall return the error code Packet Too Long (0x45). If the periodic advertising
 * train is on the LE Coded PHY, then the S=8 coding shall be assumed.
 *
 * If the response slot identified by the Response_Slot parameter has passed by the time
 * this command is received by the Controller, the Controller shall return the error code
 * Too Late (0x46) and discard the Response_Data parameter.
 * If Response_Subevent exceeds numSubevents of the periodic advertising train, or the
 * Controller is not synchronized with the subevent, then the Controller should return the
 * error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Periodic_Advertising_Response_Data command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_response_data(const sdc_hci_cmd_le_set_periodic_adv_response_data_t * p_params,
                                                      sdc_hci_cmd_le_set_periodic_adv_response_data_return_t * p_return);

/** @brief LE Set Periodic Sync Subevent.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.127
 *
 * This command is used to instruct the Controller to synchronize with a subset of the
 * subevents within a PAwR train identified by the Sync_Handle parameter, listen for
 * packets sent by the peer device and pass any received data up to the Host. If the
 * Controller is synchronized with any subevents that are not in the subset of subevents in
 * this command, then the Controller shall no longer synchronize with those subevents.
 *
 * The Periodic_Advertising_Properties parameter indicates which fields should be
 * included in the AUX_SYNC_SUBEVENT_RSP PDUs.
 *
 * The Num_Subevents_To_Sync parameter identifies the number of values in the sub-
 * events parameter.
 *
 * The Subevents arrayed parameter identifies the subevents that the Controller shall
 * synchronize with.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Sync_Subevent command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_sync_subevent(const sdc_hci_cmd_le_set_periodic_sync_subevent_t * p_params,
                                                  sdc_hci_cmd_le_set_periodic_sync_subevent_return_t * p_return);

/** @brief LE Extended Create Connection [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.66
 *
 * This command is used to create a single ACL connection, with the local device in the
 * Central role, to a connectable advertiser. The command is also used to create a single
 * ACL connection between a periodic advertiser and a synchronized device.
 * If a connection is created with the local device in the Peripheral role while this command
 * is pending, then this command remains pending.
 *
 * The Advertising_Handle parameter is used to identify the periodic advertising train.
 *
 * The Subevent parameter is used to identify the subevent where a connection
 * request shall be initiated from a periodic advertising train. The Host may use
 * this subevent whether or not the Controller has requested data for it using the
 * HCI_LE_Periodic_Advertising_Subevent_Data_Request event.
 *
 * The Advertising_Handle and Subevent parameters shall be set to 0xFF if these
 * parameters are not used. If the Host sets one but not both of these to 0xFF, then the
 * Controller shall return an error which should use the error code Invalid HCI Command
 * Parameters (0x12).
 *
 * If the Advertising_Handle and Subevent parameters are set to values other than 0xFF,
 * then the Initiator_Filter_Policy, Scan_Interval[i], and Scan_Window[i] parameters shall
 * be ignored.
 *
 * The Initiator_Filter_Policy parameter is used to determine whether the Filter Accept
 * List is used and whether to process decision PDUs and other advertising PDUs. If the
 * Filter Accept List is used or the policy allows decision PDUs to be processed (or both),
 * then the device to connect to is the first one that sends an advertisement received
 * successfully by the local device and that is on the list or that is accepted by the decision
 * instructions (see Section 7.8.145), as specified by the policy. If the Filter Accept List is
 * not used, then the Peer_Address_Type and the Peer_Address parameters specify the
 * address type and address of the advertising device to connect to for advertisements not
 * using decision PDUs. If Initiator_Filter_Policy is set to 0x03, then devices on the Filter
 * Accept List shall still be processed using the decision instructions.
 *
 * The Own_Address_Type parameter indicates the type of address being used in the
 * connection request packets.
 *
 * The Peer_Address_Type parameter indicates the type of address used in the
 * connectable advertisement sent by the peer.
 *
 * The Peer_Address parameter indicates the Peer’s Public Device Address, Random
 * (static) Device Address, Non-Resolvable Private Address, or Resolvable Private
 * Address depending on the Peer_Address_Type parameter.
 *
 * The Initiating_PHYs parameter indicates the PHY(s) on which the advertising packets
 * should be received on the primary advertising physical channel and the PHYs for
 * which connection parameters have been specified. The Host may enable one or more
 * initiating PHYs. If the Host specifies a PHY that is not supported by the Controller,
 * including a bit that is reserved for future use, the latter should return the error code
 * Unsupported Feature or Parameter Value (0x11). If the Host sets Advertising_Handle to
 * a value other than 0xFF and does not include the PHY used for the specified periodic
 * advertising train in Initiating_PHYs, the Controller shall return an error which should use
 * the error code Invalid HCI Command Parameters (0x12). The array elements of the
 * arrayed parameters are ordered in the same order as the set bits in the Initiating_PHYs
 * parameter, starting from bit 0. The number of array elements is determined by the
 * number of bits set in the Initiating_PHYs parameter. When a connectable advertisement
 * is received and a connection request is sent on one PHY, scanning on any other PHYs
 * is terminated.
 *
 * The Scan_Interval[i] and Scan_Window[i] parameters are recommendations from the
 * Host on how long (Scan_Window[i]) and how frequently (Scan_Interval[i]) the Controller
 * should scan (see [Vol 6] Part B, Section 4.5.3); however the frequency and length
 * of the scan is implementation specific. If the requested scan cannot be supported by
 * the implementation, the Controller shall return the error code Invalid HCI Command
 * Parameters (0x12). If bit 1 is set in Initiating_PHYs, the values for the LE 2M PHY shall
 * be ignored.
 *
 * The Connection_Interval_Min[i] and Connection_Interval_Max[i] parameters define the
 * minimum and maximum allowed connection interval. The Connection_Interval_Min[i]
 * parameter shall not be greater than the Connection_Interval_Max[i] parameter.
 *
 * The Max_Latency[i] parameter defines the maximum allowed Peripheral latency (see
 * [Vol 6] Part B, Section 4.5.1).
 *
 * The Supervision_Timeout[i] parameter defines the link supervision timeout for the
 * connection. The Supervision_Timeout[i] in milliseconds shall be larger than (1 +
 * Max_Latency[i]) × Connection_Interval_Max[i] × 2, where Connection_Interval_Max[i]
 * is given in milliseconds (see [Vol 6] Part B, Section 4.5.2).
 *
 * The Min_CE_Length[i] and Max_CE_Length[i] parameters provide the Controller
 * with the expected minimum and maximum length of the connection events. The
 * Min_CE_Length[i] parameter shall be less than or equal to the Max_CE_Length[i]
 * parameter. The Controller is not required to use these values.
 *
 * Where the connection is made on a PHY whose bit is not set in the
 * Initiating_PHYs parameter, the Controller shall use the Connection_Interval_Min[i],
 * Connection_Interval_Max[i], Max_Latency[i], Supervision_Timeout[i], Min_CE_Length[i],
 * and Max_CE_Length[i] parameters for an implementation-chosen PHY whose bit is set
 * in the Initiating_PHYs parameter.
 *
 * If the Host issues this command when another HCI_LE_Extended_Create_Connection
 * command is pending in the Controller, the Controller shall return the error code
 * Command Disallowed (0x0C).
 * If the local device is already connected to the same device address as the advertiser
 * (including two different Resolvable Private Addresses that resolve to the same IRK),
 * then the Controller shall return an error which should use the error code Connection
 * Already Exists (0x0B).
 *
 * If the Own_Address_Type parameter is set to 0x00 and the device does not have a
 * public address, the Controller should return an error code which should be Invalid HCI
 * Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x01 and the random address for the
 * device has not been initialized using the HCI_LE_Set_Random_Address command, the
 * Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x02, the Initiator_Filter_Policy parameter
 * is set to 0x00, the Controller's resolving list did not contain a matching entry, and the
 * device does not have a public address, the Controller should return an error code which
 * should be Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x02, the Initiator_Filter_Policy parameter
 * is not set to 0x00, and the device does not have a public address, the Controller should
 * return an error code which should be Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x03, the Initiator_Filter_Policy
 * parameter is set to 0x00, the Controller's resolving list did not contain a matching
 * entry, and the random address for the device has not been initialized using the
 * HCI_LE_Set_Random_Address command, the Controller shall return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * If the Own_Address_Type parameter is set to 0x03, the Initiator_Filter_Policy parameter
 * is not set to 0x00, and the random address for the device has not been initialized using
 * the HCI_LE_Set_Random_Address command, the Controller shall return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * If the Initiating_PHYs parameter does not have at least one bit set for a PHY allowed
 * for scanning on the primary advertising physical channel, the Controller shall return the
 * error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command and the Controller has insufficient resources to handle
 * any more connections, the Controller shall return the error code Rejected due to Limited
 * Resources (0x0D).
 *
 * If the Controller does not support the Decision-Based Advertising Filtering feature and
 * the Host issues this command with Initiator_Filter_Policy set to a value other than 0x00
 * or 0x01, the Controller shall return an error code which should be Unsupported Feature
 * or Parameter Value (0x11).
 * Missing parameters:
 *
 * When a version of this command is issued that does not include all the parameters, the
 * following values shall be used for any missing parameters:
 *
 *  Parameter                                                                   Value
 *  Advertising_Handle                                                          0xFF
 *  Subevent                                                                    0xFF
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Extended_Create_Connection command,
 * the Controller sends the HCI_Command_Status event to the Host. An
 * HCI_LE_Enhanced_Connection_Complete event shall be generated when a connection
 * is created because of this command or the connection creation procedure is
 * cancelled; until the event is generated, the command is considered pending. If
 * a connection creation is discarded, then the error code Connection Failed to be
 * Established / Synchronization Timeout (0x3E) shall be used. If a connection is created,
 * this event shall be immediately followed by an HCI_LE_Channel_Selection_Algorithm
 * event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_ext_create_conn_v2(const sdc_hci_cmd_le_ext_create_conn_v2_t * p_params);

/** @brief LE Set Periodic Advertising Parameters [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.61
 *
 * This command is used by the Host to set the parameters for periodic advertising.
 *
 * The Advertising_Handle parameter identifies the advertising set whose periodic
 * advertising parameters are being configured. If the corresponding advertising set does
 * not already exist, then the Controller shall return the error code Unknown Advertising
 * Identifier (0x42).
 *
 * The Periodic_Advertising_Interval_Min parameter shall be less than or equal to the
 * Periodic_Advertising_Interval_Max parameter. The Periodic_Advertising_Interval_Min
 * and Periodic_Advertising_Interval_Max parameters should not be the same value to
 * enable the Controller to determine the best advertising interval given other activities.
 *
 * If the periodic advertising interval range provided by the Host (Periodic_Advertising_-
 * Interval_Min, Periodic_Advertising_Interval_Max) does not overlap with the periodic
 * advertising interval range supported by the Controller, then the Controller shall return
 * an error which should use the error code Unsupported Feature or Parameter Value
 * (0x11).
 *
 * The Periodic_Advertising_Properties parameter indicates which fields should be
 * included in the advertising packet.
 *
 * The Num_Subevents parameter identifies the number of subevents that shall be
 * transmitted for each periodic advertising event. If the Num_Subevents parameter value
 * is 0x00, then the Periodic Advertising does not have responses and the Controller
 * shall ignore the Subevent_Interval, Response_Slot_Delay, Response_Slot_Spacing,
 * and Num_Response_Slots parameters. If Num_Subevents is greater than 0, then the
 * Periodic Advertising is PAwR.
 *
 * The Subevent_Interval parameter identifies the time between the subevents
 * of PAwR. The Subevent_Interval shall be less than or equal to the
 * Periodic_Advertising_Interval_Min divided by the Num_Subevents of the advertising
 * set. If Num_Subevents is set to 1, then the Controller shall ignore Subevent_Interval
 * and uses of Subevent_Interval in the next two paragraphs shall be replaced by
 * Periodic_Advertising_Interval_Max.
 *
 * The Response_Slot_Delay parameter identifies the time between the start of the
 * advertising packet at the start of a subevent and the start of the first response slot.
 * The Response_Slot_Delay shall be less than the Subevent_Interval.
 *
 * The Response_Slot_Spacing parameter identifies the time between the start of two
 * consecutive response slots. The Response_Slot_Spacing shall be less than or equal
 * to 10 × (Subevent_Interval − Response_Slot_Delay) ÷ Num_Response_Slots. If the
 * Num_Response_Slots parameter is set to 1, then the Controller shall ignore the
 * Response_Slot_Spacing parameter.
 *
 * The Num_Response_Slots parameter identifies the number of response slots in a
 * subevent.
 *
 * If the advertising set identified by the Advertising_Handle specified scannable,
 * connectable, legacy, or anonymous advertising, the Controller shall return the error
 * code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command when periodic advertising is enabled for the specified
 * advertising set, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Advertising_Handle does not identify an advertising set that is already configured
 * for periodic advertising and the Controller is unable to support more periodic advertising
 * at present, the Controller shall return the error code Memory Capacity Exceeded (0x07).
 *
 * If the advertising set already contains periodic advertising data and the length of the
 * data is greater than the maximum that the Controller can transmit within a periodic
 * advertising interval of Periodic_Advertising_Interval_Max, the Controller shall return the
 * error code Packet Too Long (0x45). If advertising on the LE Coded PHY, the S=8 coding
 * shall be assumed unless the current advertising parameters require the use of S=2 for
 * an advertising physical channel, in which case the S=2 coding shall be assumed for that
 * advertising physical channel.
 * Missing parameters:
 *
 * When a version of this command is issued that does not include all the parameters, the
 * following values shall be used for any missing parameters:
 *
 *  Parameter                                                                      Value
 *  Num_Subevents                                                                  0
 *  Subevent_Interval                                                              ignored
 *  Response_Slot_Delay                                                            ignored
 *  Response_Slot_Spacing                                                          ignored
 *  Num_Response_Slots                                                             ignored
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Periodic_Advertising_Parameters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_periodic_adv_params_v2(const sdc_hci_cmd_le_set_periodic_adv_params_v2_t * p_params,
                                                  sdc_hci_cmd_le_set_periodic_adv_params_v2_return_t * p_return);

/** @brief LE Read All Local Supported Features.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.128
 *
 * This command requests the supported LE features for the Controller.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_All_Local_Supported_Features command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_all_local_supported_features(sdc_hci_cmd_le_read_all_local_supported_features_return_t * p_return);

/** @brief LE Read All Remote Features.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.129
 *
 * This command requests, from the remote device identified by the Connection_Handle,
 * the features used on the connection and the features supported by the remote device.
 * For details see [Vol 6] Part B, Section 4.6.
 *
 * This command may be issued on both the Central and Peripheral.
 *
 * The Pages_Requested parameter specifies the highest-numbered page of features
 * that the Host requires. The Controller shall obtain all pages up to the lesser of
 * Pages_Requested and the highest page number on the remote device that contains
 * at least one bit set to 1, and may obtain some or all higher-numbered pages.
 *
 * If a connection already exists between the two devices and the features have already
 * been fetched on that connection, then the Controller may use a cached copy of the
 * features.
 *
 * If the Host issues this command when another HCI_LE_Read_All_Remote_Features
 * command is pending in the Controller, then the Controller shall return the error code
 * Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Read_All_Remote_Features command,
 * the Controller shall send the HCI_Command_Status event to the Host. When the
 * Controller has completed the procedure to determine the remote features or has
 * determined that it will be using a cached copy, the Controller shall send an
 * HCI_LE_Read_All_Remote_Features_Complete event to the Host.
 *
 * The HCI_LE_Read_All_Remote_Features_Complete event contains the status of this
 * command and the parameters describing the features supported by the remote device.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_all_remote_features(const sdc_hci_cmd_le_read_all_remote_features_t * p_params);

/** @brief LE CS Read Local Supported Capabilities.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.130
 *
 * This command allows a Host to read the CS capabilities that are supported by the
 * local Controller. This command may be used along with the local supported features to
 * provide additional details of the supported CS capabilities.
 *
 * The Num_Config_Supported parameter indicates the number of CS configurations that
 * are supported by the Controller.
 *
 * The Max_Consecutive_Procedures_Supported parameter indicates the maximum
 * number of consecutive CS procedures that are supported by the local Controller.
 *
 * The Num_Antennae_Supported parameter indicates the number of antenna elements
 * that are available for CS tone exchanges.
 * The Max_Antenna_Paths_Supported parameter indicates the maximum number of
 * antenna paths that are supported by the local Controller for CS tone exchanges.
 *
 * The Roles_Supported parameter indicates the CS roles that are supported by the local
 * Controller.
 *
 * The Modes_Supported parameter indicates the optional CS modes that are supported
 * by the local Controller.
 *
 * The RTT_Capability, RTT_AA_Only_N, RTT_Sounding_N, and the
 * RTT_Random_Sequence_N parameters indicate the time-of-flight accuracy as
 * described in [Vol 6] Part B, Section 2.4.2.44.
 *
 * The NADM_Sounding_Capability and NADM_Random_Capability parameters indicate
 * the support by the local Controller for reporting Normalized Attack Detector Metric
 * (NADM) when a CS_SYNC with a sounding sequence or random sequence is received.
 *
 * The CS_SYNC_PHYs_Supported parameter indicates the optional transmit and receive
 * PHYs that are supported by the local Controller for CS_SYNC exchanges as described
 * in [Vol 6] Part H, Section 4.3.
 *
 * The Subfeatures_Supported parameter indicates which of the following optional
 * subfeatures are supported by the local Controller:
 *
 * • A Frequency Actuation Error of zero for all allowed CS channels relative to mode-0
 *   transmissions when in the reflector role as described in [Vol 6] Part A, Section 3.5.
 * • Channel Selection Algorithm #3c as described in [Vol 6] Part H, Section 4.1.4.2.
 * • Phase-based ranging from a sounding sequence as described in [Vol 6] Part H,
 *   Section 3.3.1.
 *
 * The T_IP1_Times_Supported, T_IP2_Times_Supported, T_FCS_Times_Supported,
 * T_PM_Times_Supported, and T_SW_Time_Supported parameters indicate the
 * supported optional time durations used in CS steps as described in [Vol 6] Part H,
 * Section 4.3.
 *
 * The TX_SNR_Capability parameter indicates the supported SNR levels used for the
 * CS_SYNC packets used in mode-1 and mode-3 steps as described in [Vol 6] Part A,
 * Section 3.1.3.
 *
 * If the Host issues this command when the Channel Sounding (Host Support) feature bit
 * (see [Vol 6] Part B, Section 4.6.33.4) is not set, then the Controller shall return the error
 * code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_CS_Read_Local_Supported_Capabilities command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_read_local_supported_capabilities(sdc_hci_cmd_le_cs_read_local_supported_capabilities_return_t * p_return);

/** @brief LE CS Read Remote Supported Capabilities.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.131
 *
 * This command allows a Host to query the CS capabilities that are supported by
 * the remote Controller. If no Channel Sounding Capability Exchange procedure has
 * been initiated on the ACL connection specified by the Connection_Handle and if no
 * prior HCI_LE_CS_Write_Cached_Remote_Supported_Capabilities command has been
 * issued by the Host, then the Controller shall initiate a Channel Sounding Capabilities
 * Exchange procedure on the ACL. Otherwise, the Controller may use a cached copy of
 * the capabilities of the remote device.
 *
 * If this command is issued on the Central or Peripheral before the devices have
 * performed the Feature Exchange procedure, then the Controller of the Central or
 * Peripheral shall complete that procedure before initiating the Channel Sounding
 * Capability Exchange procedure (see [Vol 6] Part B, Section 5.1.24).
 *
 * If the Host issues this command when the local or remote Channel Sounding (Host
 * Support) feature bit (see [Vol 6] Part B, Section 4.6.33.4) is not set, then the Controller
 * shall return the error code Command Disallowed (0x0C).
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * The Host may store a copy of the remote device’s capabilities and write the remote
 * capabilities in the local Controller when it reconnects to the same remote device by
 * using the HCI_LE_CS_Write_Cached_Remote_Supported_Capabilities command.
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_CS_Read_Remote_Supported_Capabilities
 * command, the Controller shall send the HCI_Command_Status event to the
 * Host. When the Controller has completed the Channel Sounding Capability
 * Exchange procedure with the remote Controller or has a cached copy of
 * the capabilities of the remote Controller, the Controller shall generate an
 * HCI_LE_CS_Read_Remote_Supported_Capabilities_Complete event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_read_remote_supported_capabilities(const sdc_hci_cmd_le_cs_read_remote_supported_capabilities_t * p_params);

/** @brief LE CS Write Cached Remote Supported Capabilities.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.132
 *
 * This command allows a Host to write the cached copy of the CS capabilities
 * that are supported by the remote Controller for the connection identified by the
 * Connection_Handle parameter.
 *
 * The Num_Config_Supported parameter indicates the number of CS configurations that
 * are supported by the remote Controller.
 *
 * The Max_Consecutive_Procedures_Supported parameter indicates the maximum
 * number of consecutive CS procedures that are supported by the remote Controller.
 *
 * The Num_Antennae_Supported parameter indicates the number of antenna elements
 * that are available for CS tone exchanges.
 * The Max_Antenna_Paths_Supported parameter indicates the maximum number of
 * antenna paths that are supported by the local Controller for CS tone exchanges.
 *
 * The Roles_Supported parameter indicates the CS roles that are supported by the
 * remote Controller.
 *
 * The Modes_Supported parameter indicates the optional CS modes that are supported
 * by the remote Controller.
 *
 * The RTT_Capability, RTT_AA_Only_N, RTT_Sounding_N, and the
 * RTT_Random_Sequence_N parameters indicate the time-of-flight accuracy as
 * described in [Vol 6] Part B, Section 2.4.2.44.
 *
 * The NADM_Sounding_Capability and NADM_Random_Capability parameters indicate
 * the support by the remote Controller for reporting Normalized Attack Detector Metric
 * (NADM) when a CS_SYNC with a sounding sequence or random sequence is received.
 *
 * The CS_SYNC_PHYs_Supported parameter indicates the optional transmit and receive
 * PHYs that are supported by the remote Controller for CS_SYNC exchanges as
 * described in [Vol 6] Part H, Section 4.3.
 *
 * The Subfeatures_Supported parameter indicates which of the following optional
 * subfeatures are supported by the remote Controller:
 *
 * • A Frequency Actuation Error of zero for all allowed CS channels relative to mode-0
 *   transmissions when in the reflector role as described in [Vol 6] Part A, Section 3.5.
 * • Channel Selection Algorithm #3c as described in [Vol 6] Part H, Section 4.1.4.2.
 * • Phase-based ranging from a sounding sequence as described in [Vol 6] Part H,
 *   Section 3.3.1.
 *
 * The T_IP1_Times_Supported, T_IP2_Times_Supported, T_FCS_Times_Supported,
 * T_PM_Times_Supported, and T_SW_Time_Supported parameters indicate the
 * supported optional time durations used in CS steps as described in [Vol 6] Part H,
 * Section 4.3.
 *
 * The TX_SNR_Capability parameter indicates the supported SNR levels used for the
 * CS_SYNC packets used in mode-1 and mode-3 steps as described in [Vol 6] Part A,
 * Section 3.1.3.
 *
 * If the Host issues this command after an LL_CS_CAPABILITIES_REQ or
 * LL_CS_CAPABILITIES_RSP PDU has been received from the remote Controller, then
 * the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host issues this command after a CS configuration has been created in the local
 * Controller, then the Controller shall return the error code Command Disallowed (0x0C).
 * If the Host issues this command when the Channel Sounding (Host Support) feature bit
 * (see [Vol 6] Part B, Section 4.6.33.4) is not set, then the Controller shall return the error
 * code Command Disallowed (0x0C).
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL the Controller shall return the error code Unknown
 * Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_CS_Write_Cached_Remote_Supported_Capabilities command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_write_cached_remote_supported_capabilities(const sdc_hci_cmd_le_cs_write_cached_remote_supported_capabilities_t * p_params,
                                                                     sdc_hci_cmd_le_cs_write_cached_remote_supported_capabilities_return_t * p_return);

/** @brief LE CS Security Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.133
 *
 * This command is used by a Host to start or restart the Channel Sounding Security
 * Start procedure in the local Controller for the ACL connection identified by the
 * Connection_Handle parameter.
 *
 * If the Host issues this command on a Connection_Handle where the Controller is the
 * Peripheral, then the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the connection identified by the Connection_Handle parameter is not encrypted, then
 * the Controller shall return the error code Insufficient Security (0x2F).
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * If the Host issues this command when the Channel Sounding (Host Support) feature bit
 * (see [Vol 6] Part B, Section 4.6.33.4) is not set, then the Controller shall return the error
 * code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_CS_Security_Enable command, the
 * Controller shall send the HCI_Command_Status event to the Host. When the Controller
 * has completed the Channel Sounding Security Start procedure with the remote
 * Controller, the Controller shall generate an LE_CS_Security_Enable_Complete event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_security_enable(const sdc_hci_cmd_le_cs_security_enable_t * p_params);

/** @brief LE CS Set Default Settings.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.134
 *
 * This command is used by a Host to set default CS settings in the local Controller for the
 * connection identified by the Connection_Handle parameter. The default settings specify
 * that all roles are disabled in a Controller and CS_SYNC_Antenna_Selection is set to
 * 0x01.
 *
 * The Role_Enable parameter is used to enable or disable the CS roles in the local
 * Controller. If the Host issues this command to disable a Role for which a valid CS
 * configuration is present, then the Controller shall return the error code Invalid HCI
 * Command Parameters (0x12).
 *
 * The CS_SYNC_Antenna_Selection parameter indicates the antenna identifiers to be
 * used for transmitting and receiving CS_SYNC packets.
 *
 * If the Role_Enable parameter is used to enable an unsupported role or the
 * CS_SYNC_Antenna_Selection parameter indicates an unsupported antenna identifier,
 * then the Controller shall return the error code Unsupported Feature or Parameter Value
 * (0x11).
 *
 * The Max_TX_Power parameter is used to set the maximum output power, EIRP, to be
 * used for all CS transmissions. If the value provided in this parameter is higher than
 * the maximum output power supported by the Controller, then the Controller shall use
 * the maximum output power that it supports. If the Controller is unable to use the exact
 * output power requested by the Host, then the Controller shall use an output power that
 * is lower but closest to the requested value.
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_CS_Set_Default_Settings command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_set_default_settings(const sdc_hci_cmd_le_cs_set_default_settings_t * p_params,
                                               sdc_hci_cmd_le_cs_set_default_settings_return_t * p_return);

/** @brief LE CS Read Remote FAE Table.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.135
 *
 * This command is used by a Host to read the per-channel mode-0 Frequency Actuation
 * Error table of the remote Controller.
 *
 * If the remote Controller supports a Frequency Actuation Error of zero relative to its
 * mode-0 transmissions in the reflector role (No_FAE bit set as described in [Vol 6]
 * Part B, Section 2.4.2.44), then the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * The Host may store a copy of the remote device’s Frequency Actuation
 * Error table and write the remote Frequency Actuation Error table in the
 * local Controller when it reconnects to the same remote device by using the
 * HCI_LE_CS_Write_Cached_Remote_FAE_Table command.
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_CS_Read_Remote_FAE_Table command,
 * the Controller shall send the HCI_Command_Status event to the Host. When
 * the Controller has completed the Channel Sounding Mode-0 FAE Table
 * Request procedure with the remote Controller, the Controller shall generate an
 * LE_CS_Read_Remote_FAE_Table_Complete event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_read_remote_fae_table(const sdc_hci_cmd_le_cs_read_remote_fae_table_t * p_params);

/** @brief LE CS Write Cached Remote FAE Table.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.136
 *
 * This command is used by a Host to write a cached copy of the per-channel mode-0
 * Frequency Actuation Error table of the remote device in the local Controller.
 *
 * If the remote Controller supports a Frequency Actuation Error of zero relative to its
 * mode-0 transmissions in the reflector role (No_FAE bit set as described in [Vol 6]
 * Part B, Section 2.4.2.44), then the Controller shall return the error code Unsupported
 * Feature or Parameter Value (0x11).
 *
 * If the Host issues this command after an LL_CS_FAE_RSP PDU has been received
 * from the remote Controller, then the Controller shall return the error code Command
 * Disallowed (0x0C).
 *
 * If the Host issues this command after a CS configuration has been created in the local
 * Controller, then the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_CS_Write_Cached_Remote_FAE_Table command has completed,
 * an HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_write_cached_remote_fae_table(const sdc_hci_cmd_le_cs_write_cached_remote_fae_table_t * p_params,
                                                        sdc_hci_cmd_le_cs_write_cached_remote_fae_table_return_t * p_return);

/** @brief LE CS Create Config.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.137
 *
 * This command is used by a Host to create a new CS configuration or update an
 * existing CS configuration with the identifier Config_ID on the connection identified by
 * the Connection_Handle in the local and/or the remote Controller.
 *
 * When the Create_Context parameter is set to 0x00, the CS configuration is written
 * only in the local Controller. Otherwise when set to 0x01, the CS configuration is written
 * in both the local and remote Controllers using the Channel Sounding Configuration
 * procedure.
 *
 * The Main_Mode_Type and the Sub_Mode_Type parameters indicate the CS modes
 * (see [Vol 6] Part H, Table 4.11 for valid combinations) to be used during the CS
 * procedure for the specified CS configuration.
 *
 * The Min_Main_Mode_Steps and Max_Main_Mode_Steps parameters indicate the
 * range of main mode CS steps to be executed before a submode CS step is executed
 * during the CS procedure. When the Sub_Mode_Type parameter is set to 0xFF,
 * the Min_Main_Mode_Steps and Max_Main_Mode_Steps parameters are reserved for
 * future use.
 * The Main_Mode_Repetition parameter indicates the number of main mode CS steps
 * repeated from the last CS subevent at the beginning of the current CS subevent.
 *
 * The Mode_0_Steps parameter indicates the number of mode-0 CS steps to be included
 * at the beginning of each CS subevent.
 *
 * The Role parameter indicates the CS role for the local Controller for the specified CS
 * configuration. The RTT_Type parameter indicates the RTT variant to be used during the
 * CS procedure, and the CS_SYNC_PHY parameter indicates the PHY to be used for
 * CS_SYNC exchanges during the CS procedure for the specified CS configuration.
 *
 * The Channel_Map parameter indicates the channels to be used or unused during the
 * CS procedure, and the Channel_Map_Repetition parameter indicates the number of
 * times the channels specified by Channel_Map are to be repeated for non-mode-0 steps
 * during the CS procedure (see [Vol 6] Part H, Section 4.1.4).
 *
 * The Channel_Selection_Type parameter indicates the Channel Selection Algorithm
 * to be used during the CS procedure for non-mode-0 steps. When the
 * Channel_Selection_Type is set to 0x01, the Ch3c_Shape and the Ch3c_Jump
 * parameters shall each be set to the selected shape and channels to be skipped
 * as described in [Vol 6] Part H, Section 4.1.4.2. Otherwise, the Ch3c_Shape and the
 * Ch3c_Jump parameters shall be ignored.
 *
 * If the Host issues this command with parameters that are not supported by the local or
 * remote Controllers, then the Controller shall return the error code Unsupported Feature
 * or Parameter Value (0x11).
 *
 * If the Host issues this command with a Role not enabled by a prior
 * HCI_LE_CS_Set_Default_Settings command or with a set of parameters that are
 * considered an invalid configuration according to [Vol 6] Part H (e.g., a combination
 * of Main_Mode_Type and Sub_Mode_Type not specified in [Vol 6] Part H, Table 4.11),
 * then the Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command before a Channel Sounding
 * Capability Exchange procedure has been completed or an
 * HCI_LE_CS_Write_Cached_Remote_Supported_Capabilities command has been
 * issued for the connection identified by the Connection_Handle parameter, then the
 * Controller may autonomously initiate the Channel Sounding Capability Exchange
 * procedure. Otherwise, the Controller shall return the error code Command Disallowed
 * (0x0C).
 *
 * If the Host issues this command to update a CS configuration identified by the
 * Config_ID parameter that is already enabled using the HCI_LE_CS_Procedure_Enable
 * command, then the Controller shall return the error code Command Disallowed (0x0C).
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_CS_Create_Config command, the Controller
 * shall send the HCI_Command_Status event to the Host. When the Controller has
 * completed the Channel Sounding Configuration procedure with the remote Controller
 * or when the Create_Context parameter is set to 0x00, the Controller shall generate an
 * LE_CS_Config_Complete event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_create_config(const sdc_hci_cmd_le_cs_create_config_t * p_params);

/** @brief LE CS Remove Config.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.138
 *
 * This command is used to remove a CS configuration identified by Config_ID from
 * the local Controller for the connection identified by the Connection_Handle parameter.
 * When the Host issues this command, the local Controller shall initiate a Channel
 * Sounding Configuration procedure to remove the CS configuration from both the
 * local and remote device. The Controller shall delete any CS procedure related
 * parameters set using the HCI_LE_CS_Set_Procedure_Parameters command for the
 * CS configuration identified by Config_ID.
 *
 * If the CS configuration corresponding to Config_ID does not exist, then the Controller
 * shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command when one or more CS procedures have been enabled
 * using the HCI_LE_CS_Procedure_Enable command, then the Controller shall return the
 * error code Command Disallowed (0x0C).
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_CS_Remove_Config command, the
 * Controller shall send the HCI_Command_Status event to the Host. When the
 * Controller has completed the Channel Sounding Configuration procedure to disable the
 * configuration, the Controller shall generate an LE_CS_Config_Complete event.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_remove_config(const sdc_hci_cmd_le_cs_remove_config_t * p_params);

/** @brief LE CS Set Channel Classification.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.139
 *
 * This command is used by a Host to update the channel classification based on its
 * local information. This channel classification persists until overwritten with a subsequent
 * HCI_LE_CS_Set_Channel_Classification command or until the Controller is reset. The
 * Controller may combine the channel classification information provided by the Host
 * along with local channel classification information to send an updated CS channel map
 * to the remote Controller.
 *
 * If this command is used, then the Host should send updates within 10 seconds of
 * knowing that the CS channel classification has changed.
 *
 * If the Host issues this command less than 1 second after the previous time it issued this
 * command, then the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the Channel_Classification parameter enables channels that are reserved for future
 * use or enables fewer than 15 channels, then the Controller shall return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_CS_Set_Channel_Classification command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_set_channel_classification(const sdc_hci_cmd_le_cs_set_channel_classification_t * p_params);

/** @brief LE CS Set Procedure Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.140
 *
 * This command is used by a Host to set the parameters for the scheduling of
 * one or more CS procedures by the local Controller, with the remote device for
 * the CS configuration identified by Config_ID and the connection identified by the
 * Connection_Handle parameter.
 *
 * The Max_Procedure_Len parameter indicates the maximum duration of each CS
 * procedure. The Min_Procedure_Interval and Max_Procedure_Interval parameters
 * indicate the minimum and maximum interval period between consecutive
 * CS procedures. The Max_Procedure_Count parameter indicates the maximum
 * number of consecutive CS procedures to be scheduled as part of this
 * measurement. If Max_Procedure_Count is set to 1, then Min_Procedure_Interval and
 * Max_Procedure_Interval shall be ignored.
 *
 * The Min_Subevent_Len and Max_Subevent_Len parameters indicate the minimum and
 * maximum duration of each CS subevent during the CS procedure.
 *
 * The values for Min_Procedure_Interval, Max_Procedure_Interval, Min_Subevent_Len,
 * and Max_Subevent_Len are recommendations to the Controller which it may ignore.
 *
 * The Tone_Antenna_Config_Selection parameter indicates the Antenna Configuration
 * Index to be used in the CS procedure.
 * The power delta value Tx_Power_Delta indicates the recommended difference between
 * the remote device’s power level for the CS tones and CS_SYNC packets, and the
 * power level for the PHY indicated by the PHY parameter. If the resulting power level
 * goes below the minimum or goes above the maximum supported transmit power levels
 * of the remote device, then the Controller may adjust the requested power delta value.
 *
 * The Preferred_Peer_Antenna parameter indicates the preferred peer-ordered antenna
 * elements to be used by the remote device for the antenna configuration denoted by
 * the Tone_Antenna_Config_Selection parameter. The number of bits set in this field
 * shall be greater than or equal to the number of antenna elements denoted by the
 * Tone_Antenna_Config_Selection parameter.
 *
 * The SNR_Control_Initiator parameter indicates the SNR control adjustment for the
 * CS_SYNC transmissions of the initiator.
 *
 * The SNR_Control_Reflector parameter indicates the SNR control adjustment for the
 * CS_SYNC transmissions of the reflector.
 *
 * If the Host issues this command with parameters that exceed the CS capabilities or any
 * coexistence constraints, then the Controller shall return the error code Rejected Due to
 * Limited Resources (0x0D).
 *
 * If the CS configuration corresponding to Config_ID does not exist or is removed using
 * the HCI_LE_CS_Remove_Config command, then the Controller shall return the error
 * code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command when a CS procedure is already enabled for
 * the specified Config_ID in the Controller using the HCI_LE_CS_Procedure_Enable
 * command, then the Controller shall return the error code Command Disallowed (0x0C).
 *
 * The parameters specified by this command for the CS configuration identified
 * by the Config_ID parameter become invalid after the Host issues the
 * HCI_LE_CS_Remove_Config command for the given Config_ID.
 *
 * If the number of channels available for Channel Sounding before the start of a new CS
 * procedure measurement is less than 15, then the Controller shall return the error code
 * Insufficient Channels (0x48).
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_CS_Set_Procedure_Parameters command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_set_procedure_params(const sdc_hci_cmd_le_cs_set_procedure_params_t * p_params,
                                               sdc_hci_cmd_le_cs_set_procedure_params_return_t * p_return);

/** @brief LE CS Procedure Enable.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.141
 *
 * This command is used by a Host to enable or disable the scheduling of CS procedures
 * by the local Controller, with the remote device for the connection identified by the
 * Connection_Handle parameter.
 *
 * If the Host issues this command to enable a CS configuration identified by the
 * Config_ID parameter before a corresponding HCI_LE_CS_Set_Procedure_Parameters
 * command has been issued for the same Config_ID, then the Controller shall return the
 * error code Command Disallowed (0x0C).
 *
 * If the CS configuration corresponding to Config_ID does not exist (or has been removed
 * using the HCI_LE_CS_Remove_Config command), then the Controller shall return the
 * error code Invalid HCI Command Parameters (0x12).
 *
 * If the CS procedure parameters associated with the given Config_ID exceed any
 * scheduling or coexistence constraints at the time this command is issued, then the
 * Controller shall return the error code Connection Rejected Due to Limited Resources
 * (0x0D).
 *
 * If the Host issues this command to enable a CS configuration identified by the
 * Config_ID parameter that is already enabled using the HCI_LE_CS_Procedure_Enable
 * command, then the Controller shall return the error code Command Disallowed (0x0C).
 *
 * If the number of channels available for Channel Sounding before the start of a new CS
 * procedure is less than 15, then the Controller shall return the error code Insufficient
 * Channels (0x48).
 *
 * If the Host sends this command with a Connection_Handle that does not exist, or the
 * Connection_Handle is not for an ACL, then the Controller shall return the error code
 * Unknown Connection Identifier (0x02).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_CS_Procedure_Enable command, the
 * Controller shall send the HCI_Command_Status event to the Host. When
 * the locally initiated Channel Sounding Start procedure has completed or
 * when the Controller has received the LL_CS_IND PDU, it shall generate
 * an LE_CS_Procedure_Enable_Complete event. When the Host has issued a
 * command to disable an active CS procedure, the Controller shall generate an
 * LE_CS_Procedure_Enable_Complete event after any pending CS subevent results
 * have been sent to the Host and the LL_CS_TERMINATE_RSP PDU has been
 * successfully sent or received.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_procedure_enable(const sdc_hci_cmd_le_cs_procedure_enable_t * p_params);

/** @brief LE CS Test.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.142
 *
 * This command is used to start a CS test where the Implementation Under Test (IUT)
 * is placed in the role of either the initiator or reflector. The first mode-0 channel in the
 * list is used as the starting channel for the test. At the beginning of any test, the IUT
 * in the reflector role shall listen on the first mode-0 channel until it receives the first
 * transmission from the initiator. Similarly, with the IUT in the initiator role, the tester
 * will start by listening on the first mode-0 channel and the IUT shall transmit on that
 * channel for the first half of the first CS step. Thereafter, the parameters of this command
 * describe the required transmit and receive behavior for the CS test.
 *
 * This command is used to schedule a single CS procedure that consists of one or more
 * CS subevents. After the channels contained in the Channel map or the Channel array in
 * case of an override have been used the number of times indicated by Channel_Map_-
 * Repetition to schedule CS steps, or the maximum number of allowed steps in a CS
 * procedure has been reached, or the maximum number of subevents indicated by
 * the Max_Num_Subevents parameter (if non-zero) has been reached, or the maximum
 * number of allowed subevents in a CS procedure has been reached, the CS procedure
 * will end.
 *
 * The Main_Mode_Type and the Sub_Mode_Type parameters indicate the CS modes to
 * be used during the CS procedure for the specified CS configuration.
 *
 * The Main_Mode_Repetition parameter indicates the number of main mode CS steps
 * repeated from the last CS subevent at the beginning of the current CS subevent.
 *
 * The Mode_0_Steps parameter indicates the number of mode-0 CS steps to be included
 * at the beginning of each CS subevent.
 *
 * The Role parameter indicates the CS role for the local Controller.
 *
 * The RTT_Type parameter indicates the RTT type and payload length to be used during
 * the CS procedure.
 *
 * The CS_SYNC_PHY parameter indicates the PHY to be used for CS_SYNC exchanges
 * during the CS procedure.
 *
 * The CS_SYNC_Antenna_Selection parameter indicates the antenna identifier to be
 * used for transmitting and receiving CS_SYNC packets.
 *
 * The Subevent_Len parameter indicates the maximum length of a CS subevent.
 *
 * The Subevent_Interval parameter indicates the gap between the start of consecutive
 * CS subevents. If Subevent_Interval is set to zero, then only one CS subevent is
 * executed in the CS test.
 *
 * The Max_Num_Subevents parameter indicates the maximum number of subevents that
 * are in the procedure (0x00 indicates no limit). If Max_Num_Subevents is set to 0x00,
 * then the Subevent_Interval parameter shall be ignored.
 *
 * The Transmit_Power_Level parameter indicates the transmit power level used for the
 * CS procedure.
 * The T_IP1_Time, T_IP2_Time, T_FCS_Time, T_PM_Time, and T_SW_Time
 * parameters indicate the time durations used in CS steps as described in [Vol 6] Part
 * H, Section 4.3.
 *
 * The Tone_Antenna_Config_Selection parameter indicates the Antenna Configuration
 * Index used during antenna switching during the tone phases of CS steps as described
 * in [Vol 6] Part A, Section 5.3.
 *
 * The SNR_Control_Initiator parameter indicates the SNR control adjustment for the
 * CS_SYNC transmissions of the initiator.
 *
 * The SNR_Control_Reflector parameter indicates the SNR control adjustment for of the
 * CS_SYNC transmissions of the reflector.
 *
 * The DRBG_Nonce parameter specifies octets 14 and 15 of the initial value of the DRBG
 * nonce, VDRBG, used in calls to the random bit generation function described in [Vol 6]
 * Part E, Section 3.1.6. The remaining octets of the initial nonce value VDRBG are set to
 * 0x00. All octets of the initial temporal key KDRBG are set to 0x00. The most significant bit
 * of this parameter is stored in the most significant bit of the octet 14 of the DRBG nonce.
 * The least significant bit of this parameter is stored in the least significant bit of octet 15
 * of the DRBG nonce.
 *
 * The Channel_Map_Repetition field shall indicate the number of times the
 * Channel_Map is cycled through for non-mode-0 steps within a CS procedure. The
 * Channel_Map_Repetition field shall be greater than or equal to 1. The Channel_Map
 * content is selected based on the setting of the Override_Config bit 0 as described
 * below.
 *
 * The Override_Config parameter indicates which CS parameters are not derived from
 * the DRBG but determined from the Override_Parameters_Data parameter in this
 * command.
 *
 * If the Override_Config bit 2 corresponding to CS submode insertion is not set, then the
 * number of main mode CS steps to be executed before a submode CS step during the
 * CS procedure is determined using DRBG with following default values:
 *
 * • Min_Main_Mode_Steps: 6
 * • Max_Main_Mode_Steps: 10
 *
 * If the Override_Config bit 10 corresponding to the Stable Phase test is set, then
 * the procedure is replaced with the Stable Phase test as described in [Vol 6] Part F,
 * Section 2.4.
 * The Override_Parameters_Data is a variable sized object with a length indicated by
 * Override_Parameters_Length parameter whose contents are determined by the bits set
 * in the Override_Config parameter.
 *
 * An ongoing CS test can be stopped using the HCI_LE_CS_Test_End command (see
 * Section 7.8.143).
 *
 * The CS test is considered complete when all the results of the CS procedure initiated by
 * the CS test have been reported to the Host.
 *
 * If the Host issues this command when a CS test is already enabled using the
 * HCI_LE_CS_Test command and has not completed, then the Controller shall return
 * the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_CS_Test command has completed, an HCI_Command_Complete
 * event shall be generated. The Controller shall either generate the LE_CS_Subevent_-
 * Result event once or generate the LE_CS_Subevent_Result_Continue event multiple
 * times to send results from the completed CS steps to the Host.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_test(const sdc_hci_cmd_le_cs_test_t * p_params);

/** @brief LE CS Test End.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.143
 *
 * This command is used to stop any CS test that is in progress.
 *
 * If the Host issues this command when there is no prior CS test initiated using the
 * HCI_LE_CS_Test command or when a prior CS test has already been completed, then
 * the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_CS_Test_End command, the Controller
 * shall send the HCI_Command_Status event to the Host. When the Controller has
 * successfully sent all the pending CS subevent results to the Host, the Controller shall
 * generate an LE_CS_Test_End_Complete event.
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_cs_test_end(void);

/** @brief LE Set Host Feature [v2].
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.115
 *
 * This command is used by the Host to set or clear a bit controlled by the Host in the Link
 * Layer FeatureSet stored in the Controller (see [Vol 6] Part B, Section 4.6).
 *
 * The Bit_Number parameter specifies the bit position in the FeatureSet.
 *
 * The Bit_Value parameter specifies whether the feature is enabled or disabled.
 *
 * If Bit_Number specifies a feature bit that is not controlled by the Host, the Controller
 * shall return the error code Unsupported Feature or Parameter Value (0x11).
 *
 * If Bit_Value is set to 0x01 and Bit_Number specifies a feature bit that requires support of
 * a feature that the Controller does not support, the Controller shall return the error code
 * Unsupported Feature or Parameter Value (0x11).
 *
 * If the Host issues this command while the Controller has a connection to another
 * device, the Controller shall return the error code Command Disallowed (0x0C).
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Set_Host_Feature command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_host_feature_v2(const sdc_hci_cmd_le_set_host_feature_v2_t * p_params);

/** @brief LE Frame Space Update.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.151
 *
 * This command allows the Host to request a change to one or more frame space values.
 * This command may be issued on both the Central and the Peripheral.
 *
 * The Frame_Space_Min and Frame_Space_Max parameters indicate the minimum and
 * maximum allowed frame space values that the Controller should use, respectively.
 * The Frame_Space_Min parameter shall not be greater than the Frame_Space_Max
 * parameter.
 *
 * The PHYS and Spacing_Types parameters indicate which frame space values are to be
 * updated. At least one bit shall be set in the PHYS parameter and at least one bit shall
 * be set in the Spacing_Types parameter.
 *
 * The actual parameter values selected by the Link Layer may be different from the
 * parameter values provided by the Host through this command.
 *
 * If the Host issues this command with Frame_Space_Max less than the frame space
 * value, then the Controller shall ignore the value provided by the Host and shall use the
 * frame space value in use as the Frame_Space_Max.
 *
 * If the Connection_Handle parameter does not identify an active ACL connection, then
 * the Controller shall return the error code Unknown Connection Identifier (0x02).
 *
 * If the Host issues this command with Frame_Space_Max less than Frame_Space_Min,
 * then the Controller shall return the error code Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command with no bits set in the PHYS or the Spacing_Types
 * parameter, then the Controller shall reject the command and return the error code
 * Invalid HCI Command Parameters (0x12).
 *
 * If the Host issues this command with a bit set in the PHYS or the Spacing_Types
 * parameter that the Controller does not support, then the Controller shall reject the
 * command and return the error code Unsupported Feature or Parameter Value (0x11).
 * If the Host issues this command with a bit set in the PHYS or the Spacing_Types
 * parameter that the remote device does not support, then the Controller shall reject
 * the command and return the error code Unsupported Remote Feature (0x1A). The
 * Controller can determine this either because it receives a corresponding error response
 * from the remote device or because it has checked the features supported by the remote
 * device.
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Frame_Space_Update command,
 * the Controller shall send the HCI_Command_Status event to the Host.
 * When the HCI_LE_Frame_Space_Update command has completed, an
 * HCI_LE_Frame_Space_Update_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_frame_space_update(const sdc_hci_cmd_le_frame_space_update_t * p_params);

/** @brief LE Connection Rate Request.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.154
 *
 * This command is used by a Central or a Peripheral to request a change to
 * existing connection parameters (see Section 4.5.1) using the Connection Rate Update
 * procedure (see [Vol 6] Part B, Section 5.1.32) or the Connection Rate Request
 * procedure (see [Vol 6] Part B, Section 5.1.33).
 *
 * The Connection_Interval_Min and Connection_Interval_Max parameters are used to
 * define the minimum and maximum allowed connection interval.
 *
 * The Subrate_Min and Subrate_Max parameters specify the range of acceptable
 * subrating factors being requested.
 *
 * The Max_Latency parameter specifies the maximum allowed Peripheral latency in units
 * of subrated connection events. The same maximum shall apply irrespective of the
 * subrating factor actually chosen.
 *
 * The Continuation_Number parameter specifies the number of underlying connection
 * intervals to remain active after a packet other than an empty packet is transmitted or
 * received.
 *
 * The Supervision_Timeout parameter specifies the link supervision timeout for the
 * connection.
 *
 * If this command is issued on the Central, then it also sets the acceptable
 * parameters for requests from the Peripheral (see [Vol 6] Part B, Section 5.1.33).
 * The acceptable parameters set by this command override those provided via the
 * HCI_LE_Set_Default_Rate_Parameters command or any values set by previous uses
 * of this command on the same connection.
 * The Min_CE_Length and Max_CE_Length parameters provide the Controller with the
 * expected minimum and maximum length of the connection events. The Controller is not
 * required to use these values.
 *
 * If this command is issued on the Central before the devices have performed the Feature
 * Exchange procedure, then the Controller shall complete that procedure before initiating
 * the Connection Rate Request procedure.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                         Error Code
 * MC     One or more CS procedures are enabled.                            Command Disallowed
 *                                                                          (0x0C)
 * MC     A Connection Parameters Request procedure or a Connection         Command Disallowed
 *        Subrate Request procedure is in progress.                         (0x0C)
 * MC     The Connection_Handle parameter does not identify an              Unknown Connection
 *        established ACL connection.                                       Identifier (0x02)
 * MC     Subrate_Max × (Max_Latency + 1) is greater than 500.              Invalid HCI Command
 *                                                                          Parameters (0x12)
 * MC     Connection_Interval_Max × Subrate_Max × (Max_Latency + 1) is      Invalid HCI Command
 *        greater than or equal to Supervision_Timeout × 40.                Parameters (0x12)
 * MC     Connection_Interval_Max is less than Connection_Interval_Min.     Invalid HCI Command
 *                                                                          Parameters (0x12)
 * MC     Connection_Interval_Min and Connection_Interval_Max specify a     Unsupported LL
 *        range of connection intervals which is not supported by the       Parameter Value (0x20)
 *        Controller.
 * MC     Connection_Interval_Max is less than connIntervalRequired (see    Unsupported Feature or
 *        [Vol 6] Part B, Section 4.5.10).                                  Parameter Value (0x11)
 * MC     Subrate_Max is less than Subrate_Min.                             Invalid HCI Command
 *                                                                          Parameters (0x12)
 * MC     Continuation_Number is greater than or equal to Subrate_Max.      Invalid HCI Command
 *                                                                          Parameters (0x12)
 * MC     The Connection Rate (Host Support) bit is not set in the local    Unsupported Remote
 *        or remote device’s FeatureSet.                                    Feature (0x1A)
 * MC     Max_CE_Length is less than Min_CE_Length                          Invalid HCI Command
 *                                                                          Parameters (0x12)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Connection_Rate_Request command,
 * the Controller shall send the HCI_Command_Status event to the Host. An
 * HCI_LE_Connection_Rate_Change event shall be generated when the Connection
 * Rate Update procedure or the Connection Rate Request procedure has completed.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_conn_rate_request(const sdc_hci_cmd_le_conn_rate_request_t * p_params);

/** @brief LE Set Default Rate Parameters.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.155
 *
 * This command is used by the Host to set the initial values of the acceptable parameters
 * for the Connection Rate Request procedure for all future ACL connections where the
 * Controller is Central. The command does not affect any existing connection.
 *
 * The parameters have the same meanings as those in the
 * HCI_LE_Connection_Rate_Request command (see Section 7.8.154).
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                          Error Code
 * MC     Subrate_Max × (Max_Latency + 1) is greater than 500.               Invalid HCI Command
 *                                                                           Parameters (0x12)
 * MC     Connection_Interval_Max × Subrate_Max × (Max_Latency + 1) is       Invalid HCI Command
 *        greater than or equal to Supervision_Timeout × 40.                 Parameters (0x12)
 * MC     Connection_Interval_Max is less than Connection_Interval_Min.      Invalid HCI Command
 *                                                                           Parameters (0x12)
 * MC     Subrate_Max is less than Subrate_Min.                              Invalid HCI Command
 *                                                                           Parameters (0x12)
 * MC     Continuation_Number is greater than or equal to Subrate_Max.       Invalid HCI Command
 *                                                                           Parameters (0x12)
 * MC     Max_CE_Length is less than Min_CE_Length                           Invalid HCI Command
 *                                                                           Parameters (0x12)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the Controller receives the HCI_LE_Set_Default_Rate_Parameters command,
 * the Controller shall send the HCI_Command_Complete event to the Host.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_set_default_rate_params(const sdc_hci_cmd_le_set_default_rate_params_t * p_params);

/** @brief LE Read Minimum Supported Connection Interval.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.8.156
 *
 * This command is used by the Host to determine the minimum supported connection
 * interval and which other connection intervals (see [Vol 6] Part B, Section 4.5.1) the
 * Controller supports.
 *
 * Each group shall represent a set of supported connection intervals forming an
 * arithmetic sequence starting at Group_Min[i], ending at Group_Max[i], and in steps of
 * Group_Stride[i]. For example, if Group_Min[i] = 0x0005, Group_Max[i] = 0x003C, and
 * Group_Strides[i] = 0x000B, then this group indicates that the Controller supports the
 * intervals 0.625 ms, 2.000 ms, 3.375 ms, 4.750 ms, 6.125 ms, and 7.500 ms.
 *
 * The returned groups should include between them every supported value that is in ECV
 * and not in RCV but may include values in RCV. A supported value may be in more
 * than one group. The Controller shall not include any group that only contains values
 * from RCV or the minimum supported. If it would require too many groups to represent
 * all supported values, then the Controller should represent as many supported values as
 * possible.
 *
 * For example, if the Controller returns Num_Groups = 2, Group_Min[0] = 0x0005,
 * Group_Max[0] = 0x0050, Group_Strides[0] = 0x0005, Group_Min[1] = 0x0050,
 * Group_Max[1] = 0x0320, and Group_Strides[1] = 0x0008, then the Controller supports
 * all connection interval values in RCV, all values up to 10 ms that are multiples of 625 μs,
 * and all values between 10 ms and 100 ms that are multiples of 1 ms. If the Controller
 * supports any other values, it should have included more groups.
 *
 * If the Controller only supports values from RCV, then it shall set Num_Groups to zero.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_LE_Read_Minimum_Supported_Connection_Interval command has
 * completed, an HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_le_read_min_supported_conn_interval(sdc_hci_cmd_le_read_min_supported_conn_interval_return_t * p_return);

/** @} end of HCI_VS_API */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDC_HCI_CMD_LE_H__ */
