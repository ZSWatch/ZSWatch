/*
 * Copyright (c) Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sdc_hci_cmd_info_params.h
 *
 * @defgroup sdc_hci_ip SoftDevice Controller HCI Interface for Informational parameters
 * @ingroup sdc_hci
 *
 * All APIs in this header file are expected to be called from the
 * same execution priority as mpsl_low_priority_process.
 * Not doing so will lead to undefined behavior.
 *
 * @{
 */

#ifndef SDC_HCI_CMD_INFO_PARAMS_H__
#define SDC_HCI_CMD_INFO_PARAMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nrfx.h>

/**
 * @defgroup HCI_TYPES Types
 * @{
 */

/** @brief HCI IP OpCode Field values. */
enum sdc_hci_opcode_ip
{
    /** @brief See @ref sdc_hci_cmd_ip_read_local_version_information(). */
    SDC_HCI_OPCODE_CMD_IP_READ_LOCAL_VERSION_INFORMATION = 0x1001,
    /** @brief See @ref sdc_hci_cmd_ip_read_local_supported_commands(). */
    SDC_HCI_OPCODE_CMD_IP_READ_LOCAL_SUPPORTED_COMMANDS = 0x1002,
    /** @brief See @ref sdc_hci_cmd_ip_read_local_supported_features(). */
    SDC_HCI_OPCODE_CMD_IP_READ_LOCAL_SUPPORTED_FEATURES = 0x1003,
    /** @brief See @ref sdc_hci_cmd_ip_read_bd_addr(). */
    SDC_HCI_OPCODE_CMD_IP_READ_BD_ADDR = 0x1009,
};

/** @brief LMP features.
 *
 * See Core_v5.4, Vol 2, Part C, Section 3.3
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t slot3_packets : 1;
    uint8_t slot5_packets : 1;
    uint8_t encryption : 1;
    uint8_t slot_offset : 1;
    uint8_t timing_accuracy : 1;
    uint8_t role_switch : 1;
    uint8_t hold_mode : 1;
    uint8_t sniff_mode : 1;
    uint8_t previously_used : 1;
    uint8_t power_control_requests : 1;
    uint8_t cqddr : 1;
    uint8_t sco_link : 1;
    uint8_t hv2_packets : 1;
    uint8_t hv3_packets : 1;
    uint8_t u_law_log_sync_data : 1;
    uint8_t a_law_log_sync_data : 1;
    uint8_t cvsd_sync_data : 1;
    uint8_t paging_param_negiotiation : 1;
    uint8_t power_control : 1;
    uint8_t transparent_sync_data : 1;
    uint8_t flow_control_lag : 3;
    uint8_t broadcast_encryption : 1;
    uint8_t rfu1 : 1;
    uint8_t enhanced_data_rate_acl_2mbps_mode : 1;
    uint8_t enhanced_data_rate_acl_3mbps_mode : 1;
    uint8_t enhanced_inquiry_scan : 1;
    uint8_t interlaced_inquiry_scan : 1;
    uint8_t interlaced_page_scan : 1;
    uint8_t rssi_with_inquiry_results : 1;
    uint8_t ext_sco_link : 1;
    uint8_t ev4_packets : 1;
    uint8_t ev5_packets : 1;
    uint8_t rfu2 : 1;
    uint8_t afh_capable_peripheral : 1;
    uint8_t afh_classification_peripheral : 1;
    uint8_t bdedr_not_supported : 1;
    uint8_t le_supported : 1;
    uint8_t slot3_enhanced_data_rate : 1;
    uint8_t slot5_enhanced_data_rate : 1;
    uint8_t sniff_subrating : 1;
    uint8_t pause_encryption : 1;
    uint8_t afh_capable_central : 1;
    uint8_t afh_classification_central : 1;
    uint8_t enhanced_data_rate_esco_2mbps_mode : 1;
    uint8_t enhanced_data_rate_esco_3mbps_mode : 1;
    uint8_t slot3_enhanced_data_rate_esco : 1;
    uint8_t ext_inquiry_response : 1;
    uint8_t simultanious_le_and_bdedr : 1;
    uint8_t rfu3 : 1;
    uint8_t secure_simple_pairing : 1;
    uint8_t encapsulated_pdu : 1;
    uint8_t errorneous_data_reporting : 1;
    uint8_t non_flushable_packet_boundary : 1;
    uint8_t rfu4 : 1;
    uint8_t hci_link_supervision_timeout_changed_event : 1;
    uint8_t variable_inquiry_tx_power_level : 1;
    uint8_t enhanced_power_control : 1;
    uint8_t rfu5 : 1;
    uint8_t rfu6 : 1;
    uint8_t rfu7 : 1;
    uint8_t rfu8 : 1;
    uint8_t ext_features : 1;
} sdc_hci_ip_lmp_features_t;

/** @brief Supported HCI commands.
 *
 * See Core Specification, Vol 4, Part E, Section 6.27
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t hci_inquiry : 1;
    uint8_t hci_inquiry_cancel : 1;
    uint8_t hci_periodic_inquiry_mode : 1;
    uint8_t hci_exit_periodic_inquiry_mode : 1;
    uint8_t hci_create_connection : 1;
    uint8_t hci_disconnect : 1;
    uint8_t previously_used_0_6 : 1;
    uint8_t hci_create_connection_cancel : 1;
    uint8_t hci_accept_connection_request : 1;
    uint8_t hci_reject_connection_request : 1;
    uint8_t hci_link_key_request_reply : 1;
    uint8_t hci_link_key_request_negative_reply : 1;
    uint8_t hci_pin_code_request_reply : 1;
    uint8_t hci_pin_code_request_negative_reply : 1;
    uint8_t hci_change_connection_packet_type : 1;
    uint8_t hci_authentication_requested : 1;
    uint8_t hci_set_connection_encryption : 1;
    uint8_t hci_change_connection_link_key : 1;
    uint8_t hci_link_key_selection : 1;
    uint8_t hci_remote_name_request : 1;
    uint8_t hci_remote_name_request_cancel : 1;
    uint8_t hci_read_remote_supported_features : 1;
    uint8_t hci_read_remote_extended_features : 1;
    uint8_t hci_read_remote_version_information : 1;
    uint8_t hci_read_clock_offset : 1;
    uint8_t hci_read_lmp_handle : 1;
    uint8_t rfu_3_2 : 1;
    uint8_t rfu_3_3 : 1;
    uint8_t rfu_3_4 : 1;
    uint8_t rfu_3_5 : 1;
    uint8_t rfu_3_6 : 1;
    uint8_t rfu_3_7 : 1;
    uint8_t rfu_4_0 : 1;
    uint8_t hci_hold_mode : 1;
    uint8_t hci_sniff_mode : 1;
    uint8_t hci_exit_sniff_mode : 1;
    uint8_t previously_used_4_4 : 1;
    uint8_t previously_used_4_5 : 1;
    uint8_t hci_qos_setup : 1;
    uint8_t hci_role_discovery : 1;
    uint8_t hci_switch_role : 1;
    uint8_t hci_read_link_policy_settings : 1;
    uint8_t hci_write_link_policy_settings : 1;
    uint8_t hci_read_default_link_policy_settings : 1;
    uint8_t hci_write_default_link_policy_settings : 1;
    uint8_t hci_flow_specification : 1;
    uint8_t hci_set_event_mask : 1;
    uint8_t hci_reset : 1;
    uint8_t hci_set_event_filter : 1;
    uint8_t hci_flush : 1;
    uint8_t hci_read_pin_type : 1;
    uint8_t hci_write_pin_type : 1;
    uint8_t previously_used_6_4 : 1;
    uint8_t hci_read_stored_link_key : 1;
    uint8_t hci_write_stored_link_key : 1;
    uint8_t hci_delete_stored_link_key : 1;
    uint8_t hci_write_local_name : 1;
    uint8_t hci_read_local_name : 1;
    uint8_t hci_read_connection_accept_timeout : 1;
    uint8_t hci_write_connection_accept_timeout : 1;
    uint8_t hci_read_page_timeout : 1;
    uint8_t hci_write_page_timeout : 1;
    uint8_t hci_read_scan_enable : 1;
    uint8_t hci_write_scan_enable : 1;
    uint8_t hci_read_page_scan_activity : 1;
    uint8_t hci_write_page_scan_activity : 1;
    uint8_t hci_read_inquiry_scan_activity : 1;
    uint8_t hci_write_inquiry_scan_activity : 1;
    uint8_t hci_read_authentication_enable : 1;
    uint8_t hci_write_authentication_enable : 1;
    uint8_t previously_used_8_6 : 1;
    uint8_t previously_used_8_7 : 1;
    uint8_t hci_read_class_of_device : 1;
    uint8_t hci_write_class_of_device : 1;
    uint8_t hci_read_voice_setting : 1;
    uint8_t hci_write_voice_setting : 1;
    uint8_t hci_read_automatic_flush_timeout : 1;
    uint8_t hci_write_automatic_flush_timeout : 1;
    uint8_t hci_read_num_broadcast_retransmissions : 1;
    uint8_t hci_write_num_broadcast_retransmissions : 1;
    uint8_t hci_read_hold_mode_activity : 1;
    uint8_t hci_write_hold_mode_activity : 1;
    uint8_t hci_read_transmit_power_level : 1;
    uint8_t hci_read_synchronous_flow_control_enable : 1;
    uint8_t hci_write_synchronous_flow_control_enable : 1;
    uint8_t hci_set_controller_to_host_flow_control : 1;
    uint8_t hci_host_buffer_size : 1;
    uint8_t hci_host_number_of_completed_packets : 1;
    uint8_t hci_read_link_supervision_timeout : 1;
    uint8_t hci_write_link_supervision_timeout : 1;
    uint8_t hci_read_number_of_supported_iac : 1;
    uint8_t hci_read_current_iac_lap : 1;
    uint8_t hci_write_current_iac_lap : 1;
    uint8_t previously_used_11_5 : 1;
    uint8_t previously_used_11_6 : 1;
    uint8_t previously_used_11_7 : 1;
    uint8_t previously_used_12_0 : 1;
    uint8_t hci_set_afh_host_channel_classification : 1;
    uint8_t hci_le_cs_read_remote_fae_table : 1;
    uint8_t hci_le_cs_write_cached_remote_fae_table : 1;
    uint8_t hci_read_inquiry_scan_type : 1;
    uint8_t hci_write_inquiry_scan_type : 1;
    uint8_t hci_read_inquiry_mode : 1;
    uint8_t hci_write_inquiry_mode : 1;
    uint8_t hci_read_page_scan_type : 1;
    uint8_t hci_write_page_scan_type : 1;
    uint8_t hci_read_afh_channel_assessment_mode : 1;
    uint8_t hci_write_afh_channel_assessment_mode : 1;
    uint8_t rfu_13_4 : 1;
    uint8_t rfu_13_5 : 1;
    uint8_t rfu_13_6 : 1;
    uint8_t rfu_13_7 : 1;
    uint8_t rfu_14_0 : 1;
    uint8_t rfu_14_1 : 1;
    uint8_t rfu_14_2 : 1;
    uint8_t hci_read_local_version_information : 1;
    uint8_t rfu_14_4 : 1;
    uint8_t hci_read_local_supported_features : 1;
    uint8_t hci_read_local_extended_features : 1;
    uint8_t hci_read_buffer_size : 1;
    uint8_t previously_used_15_0 : 1;
    uint8_t hci_read_bd_addr : 1;
    uint8_t hci_read_failed_contact_counter : 1;
    uint8_t hci_reset_failed_contact_counter : 1;
    uint8_t hci_read_link_quality : 1;
    uint8_t hci_read_rssi : 1;
    uint8_t hci_read_afh_channel_map : 1;
    uint8_t hci_read_clock : 1;
    uint8_t hci_read_loopback_mode : 1;
    uint8_t hci_write_loopback_mode : 1;
    uint8_t hci_enable_implementation_under_test_mode : 1;
    uint8_t hci_setup_synchronous_connection_request : 1;
    uint8_t hci_accept_synchronous_connection_request : 1;
    uint8_t hci_reject_synchronous_connection_request : 1;
    uint8_t hci_le_cs_create_config : 1;
    uint8_t hci_le_cs_remove_config : 1;
    uint8_t hci_read_extended_inquiry_response : 1;
    uint8_t hci_write_extended_inquiry_response : 1;
    uint8_t hci_refresh_encryption_key : 1;
    uint8_t rfu_17_3 : 1;
    uint8_t hci_sniff_subrating : 1;
    uint8_t hci_read_simple_pairing_mode : 1;
    uint8_t hci_write_simple_pairing_mode : 1;
    uint8_t hci_read_local_oob_data : 1;
    uint8_t hci_read_inquiry_response_transmit_power_level : 1;
    uint8_t hci_write_inquiry_transmit_power_level : 1;
    uint8_t hci_read_default_erroneous_data_reporting : 1;
    uint8_t hci_write_default_erroneous_data_reporting : 1;
    uint8_t rfu_18_4 : 1;
    uint8_t rfu_18_5 : 1;
    uint8_t rfu_18_6 : 1;
    uint8_t hci_io_capability_request_reply : 1;
    uint8_t hci_user_confirmation_request_reply : 1;
    uint8_t hci_user_confirmation_request_negative_reply : 1;
    uint8_t hci_user_passkey_request_reply : 1;
    uint8_t hci_user_passkey_request_negative_reply : 1;
    uint8_t hci_remote_oob_data_request_reply : 1;
    uint8_t hci_write_simple_pairing_debug_mode : 1;
    uint8_t hci_enhanced_flush : 1;
    uint8_t hci_remote_oob_data_request_negative_reply : 1;
    uint8_t rfu_20_0 : 1;
    uint8_t rfu_20_1 : 1;
    uint8_t hci_send_keypress_notification : 1;
    uint8_t hci_io_capability_request_negative_reply : 1;
    uint8_t hci_read_encryption_key_size : 1;
    uint8_t hci_le_cs_read_local_supported_capabilities : 1;
    uint8_t hci_le_cs_read_remote_supported_capabilities : 1;
    uint8_t hci_le_cs_write_cached_remote_supported_capabilities : 1;
    uint8_t previously_used_21_0 : 1;
    uint8_t previously_used_21_1 : 1;
    uint8_t previously_used_21_2 : 1;
    uint8_t previously_used_21_3 : 1;
    uint8_t previously_used_21_4 : 1;
    uint8_t previously_used_21_5 : 1;
    uint8_t previously_used_21_6 : 1;
    uint8_t previously_used_21_7 : 1;
    uint8_t previously_used_22_0 : 1;
    uint8_t previously_used_22_1 : 1;
    uint8_t hci_set_event_mask_page_2 : 1;
    uint8_t previously_used_22_3 : 1;
    uint8_t previously_used_22_4 : 1;
    uint8_t previously_used_22_5 : 1;
    uint8_t previously_used_22_6 : 1;
    uint8_t previously_used_22_7 : 1;
    uint8_t hci_read_flow_control_mode : 1;
    uint8_t hci_write_flow_control_mode : 1;
    uint8_t hci_read_data_block_size : 1;
    uint8_t hci_le_cs_test : 1;
    uint8_t hci_le_cs_test_end : 1;
    uint8_t previously_used_23_5 : 1;
    uint8_t previously_used_23_6 : 1;
    uint8_t previously_used_23_7 : 1;
    uint8_t hci_read_enhanced_transmit_power_level : 1;
    uint8_t hci_le_cs_security_enable : 1;
    uint8_t previously_used_24_2 : 1;
    uint8_t previously_used_24_3 : 1;
    uint8_t previously_used_24_4 : 1;
    uint8_t hci_read_le_host_support : 1;
    uint8_t hci_write_le_host_support : 1;
    uint8_t hci_le_cs_set_default_settings : 1;
    uint8_t hci_le_set_event_mask : 1;
    uint8_t hci_le_read_buffer_size_v1 : 1;
    uint8_t hci_le_read_local_supported_features : 1;
    uint8_t rfu_25_3 : 1;
    uint8_t hci_le_set_random_address : 1;
    uint8_t hci_le_set_advertising_parameters : 1;
    uint8_t hci_le_read_advertising_physical_channel_tx_power : 1;
    uint8_t hci_le_set_advertising_data : 1;
    uint8_t hci_le_set_scan_response_data : 1;
    uint8_t hci_le_set_advertising_enable : 1;
    uint8_t hci_le_set_scan_parameters : 1;
    uint8_t hci_le_set_scan_enable : 1;
    uint8_t hci_le_create_connection : 1;
    uint8_t hci_le_create_connection_cancel : 1;
    uint8_t hci_le_read_filter_accept_list_size : 1;
    uint8_t hci_le_clear_filter_accept_list : 1;
    uint8_t hci_le_add_device_to_filter_accept_list : 1;
    uint8_t hci_le_remove_device_from_filter_accept_list : 1;
    uint8_t hci_le_connection_update : 1;
    uint8_t hci_le_set_host_channel_classification : 1;
    uint8_t hci_le_read_channel_map : 1;
    uint8_t hci_le_read_remote_features : 1;
    uint8_t hci_le_encrypt : 1;
    uint8_t hci_le_rand : 1;
    uint8_t hci_le_enable_encryption : 1;
    uint8_t hci_le_long_term_key_request_reply : 1;
    uint8_t hci_le_long_term_key_request_negative_reply : 1;
    uint8_t hci_le_read_supported_states : 1;
    uint8_t hci_le_receiver_test_v1 : 1;
    uint8_t hci_le_transmitter_test_v1 : 1;
    uint8_t hci_le_test_end : 1;
    uint8_t hci_le_enable_monitoring_advertisers : 1;
    uint8_t hci_le_cs_set_channel_classification : 1;
    uint8_t hci_le_cs_set_procedure_parameters : 1;
    uint8_t hci_le_cs_procedure_enable : 1;
    uint8_t hci_enhanced_setup_synchronous_connection : 1;
    uint8_t hci_enhanced_accept_synchronous_connection : 1;
    uint8_t hci_read_local_supported_codecs : 1;
    uint8_t hci_set_mws_channel_parameters : 1;
    uint8_t hci_set_external_frame_configuration : 1;
    uint8_t hci_set_mws_signaling : 1;
    uint8_t hci_set_mws_transport_layer : 1;
    uint8_t hci_set_mws_scan_frequency_table : 1;
    uint8_t hci_get_mws_transport_layer_configuration : 1;
    uint8_t hci_set_mws_pattern_configuration : 1;
    uint8_t hci_set_triggered_clock_capture : 1;
    uint8_t hci_truncated_page : 1;
    uint8_t hci_truncated_page_cancel : 1;
    uint8_t hci_set_connectionless_peripheral_broadcast : 1;
    uint8_t hci_set_connectionless_peripheral_broadcast_receive : 1;
    uint8_t hci_start_synchronization_train : 1;
    uint8_t hci_receive_synchronization_train : 1;
    uint8_t hci_set_reserved_lt_addr : 1;
    uint8_t hci_delete_reserved_lt_addr : 1;
    uint8_t hci_set_connectionless_peripheral_broadcast_data : 1;
    uint8_t hci_read_synchronization_train_parameters : 1;
    uint8_t hci_write_synchronization_train_parameters : 1;
    uint8_t hci_remote_oob_extended_data_request_reply : 1;
    uint8_t hci_read_secure_connections_host_support : 1;
    uint8_t hci_write_secure_connections_host_support : 1;
    uint8_t hci_read_authenticated_payload_timeout : 1;
    uint8_t hci_write_authenticated_payload_timeout : 1;
    uint8_t hci_read_local_oob_extended_data : 1;
    uint8_t hci_write_secure_connections_test_mode : 1;
    uint8_t hci_read_extended_page_timeout : 1;
    uint8_t hci_write_extended_page_timeout : 1;
    uint8_t hci_read_extended_inquiry_length : 1;
    uint8_t hci_write_extended_inquiry_length : 1;
    uint8_t hci_le_remote_connection_parameter_request_reply : 1;
    uint8_t hci_le_remote_connection_parameter_request_negative_reply : 1;
    uint8_t hci_le_set_data_length : 1;
    uint8_t hci_le_read_suggested_default_data_length : 1;
    uint8_t hci_le_write_suggested_default_data_length : 1;
    uint8_t hci_le_read_local_p256_public_key : 1;
    uint8_t hci_le_generate_dhkey_v1 : 1;
    uint8_t hci_le_add_device_to_resolving_list : 1;
    uint8_t hci_le_remove_device_from_resolving_list : 1;
    uint8_t hci_le_clear_resolving_list : 1;
    uint8_t hci_le_read_resolving_list_size : 1;
    uint8_t hci_le_read_peer_resolvable_address : 1;
    uint8_t hci_le_read_local_resolvable_address : 1;
    uint8_t hci_le_set_address_resolution_enable : 1;
    uint8_t hci_le_set_resolvable_private_address_timeout : 1;
    uint8_t hci_le_read_maximum_data_length : 1;
    uint8_t hci_le_read_phy : 1;
    uint8_t hci_le_set_default_phy : 1;
    uint8_t hci_le_set_phy : 1;
    uint8_t hci_le_receiver_test_v2 : 1;
    uint8_t hci_le_transmitter_test_v2 : 1;
    uint8_t hci_le_set_advertising_set_random_address : 1;
    uint8_t hci_le_set_extended_advertising_parameters : 1;
    uint8_t hci_le_set_extended_advertising_data : 1;
    uint8_t hci_le_set_extended_scan_response_data : 1;
    uint8_t hci_le_set_extended_advertising_enable : 1;
    uint8_t hci_le_read_maximum_advertising_data_length : 1;
    uint8_t hci_le_read_number_of_supported_advertising_sets : 1;
    uint8_t hci_le_remove_advertising_set : 1;
    uint8_t hci_le_clear_advertising_sets : 1;
    uint8_t hci_le_set_periodic_advertising_parameters : 1;
    uint8_t hci_le_set_periodic_advertising_data : 1;
    uint8_t hci_le_set_periodic_advertising_enable : 1;
    uint8_t hci_le_set_extended_scan_parameters : 1;
    uint8_t hci_le_set_extended_scan_enable : 1;
    uint8_t hci_le_extended_create_connection : 1;
    uint8_t hci_le_periodic_advertising_create_sync : 1;
    uint8_t hci_le_periodic_advertising_create_sync_cancel : 1;
    uint8_t hci_le_periodic_advertising_terminate_sync : 1;
    uint8_t hci_le_add_device_to_periodic_advertiser_list : 1;
    uint8_t hci_le_remove_device_from_periodic_advertiser_list : 1;
    uint8_t hci_le_clear_periodic_advertiser_list : 1;
    uint8_t hci_le_read_periodic_advertiser_list_size : 1;
    uint8_t hci_le_read_transmit_power : 1;
    uint8_t hci_le_read_rf_path_compensation : 1;
    uint8_t hci_le_write_rf_path_compensation : 1;
    uint8_t hci_le_set_privacy_mode : 1;
    uint8_t hci_le_receiver_test_v3 : 1;
    uint8_t hci_le_transmitter_test_v3 : 1;
    uint8_t hci_le_set_connectionless_cte_transmit_parameters : 1;
    uint8_t hci_le_set_connectionless_cte_transmit_enable : 1;
    uint8_t hci_le_set_connectionless_iq_sampling_enable : 1;
    uint8_t hci_le_set_connection_cte_receive_parameters : 1;
    uint8_t hci_le_set_connection_cte_transmit_parameters : 1;
    uint8_t hci_le_connection_cte_request_enable : 1;
    uint8_t hci_le_connection_cte_response_enable : 1;
    uint8_t hci_le_read_antenna_information : 1;
    uint8_t hci_le_set_periodic_advertising_receive_enable : 1;
    uint8_t hci_le_periodic_advertising_sync_transfer : 1;
    uint8_t hci_le_periodic_advertising_set_info_transfer : 1;
    uint8_t hci_le_set_periodic_advertising_sync_transfer_parameters : 1;
    uint8_t hci_le_set_default_periodic_advertising_sync_transfer_parameters : 1;
    uint8_t hci_le_generate_dhkey_v2 : 1;
    uint8_t hci_read_local_simple_pairing_options : 1;
    uint8_t hci_le_modify_sleep_clock_accuracy : 1;
    uint8_t hci_le_read_buffer_size_v2 : 1;
    uint8_t hci_le_read_iso_tx_sync : 1;
    uint8_t hci_le_set_cig_parameters : 1;
    uint8_t hci_le_set_cig_parameters_test : 1;
    uint8_t hci_le_create_cis : 1;
    uint8_t hci_le_remove_cig : 1;
    uint8_t hci_le_accept_cis_request : 1;
    uint8_t hci_le_reject_cis_request : 1;
    uint8_t hci_le_create_big : 1;
    uint8_t hci_le_create_big_test : 1;
    uint8_t hci_le_terminate_big : 1;
    uint8_t hci_le_big_create_sync : 1;
    uint8_t hci_le_big_terminate_sync : 1;
    uint8_t hci_le_request_peer_sca : 1;
    uint8_t hci_le_setup_iso_data_path : 1;
    uint8_t hci_le_remove_iso_data_path : 1;
    uint8_t hci_le_iso_transmit_test : 1;
    uint8_t hci_le_iso_receive_test : 1;
    uint8_t hci_le_iso_read_test_counters : 1;
    uint8_t hci_le_iso_test_end : 1;
    uint8_t hci_le_set_host_feature : 1;
    uint8_t hci_le_read_iso_link_quality : 1;
    uint8_t hci_le_enhanced_read_transmit_power_level : 1;
    uint8_t hci_le_read_remote_transmit_power_level : 1;
    uint8_t hci_le_set_path_loss_reporting_parameters : 1;
    uint8_t hci_le_set_path_loss_reporting_enable : 1;
    uint8_t hci_le_set_transmit_power_reporting_enable : 1;
    uint8_t hci_le_transmitter_test_v4 : 1;
    uint8_t hci_set_ecosystem_base_interval : 1;
    uint8_t hci_read_local_supported_codecs_v2 : 1;
    uint8_t hci_read_local_supported_codec_capabilities : 1;
    uint8_t hci_read_local_supported_controller_delay : 1;
    uint8_t hci_configure_data_path : 1;
    uint8_t hci_le_set_data_related_address_changes : 1;
    uint8_t hci_set_min_encryption_key_size : 1;
    uint8_t hci_le_set_default_subrate_command : 1;
    uint8_t hci_le_subrate_request_command : 1;
    uint8_t hci_le_set_extended_advertising_parameters_v2 : 1;
    uint8_t hci_le_set_decision_data : 1;
    uint8_t hci_le_set_decision_instructions : 1;
    uint8_t hci_le_set_periodic_advertising_subevent_data : 1;
    uint8_t hci_le_set_periodic_advertising_response_data : 1;
    uint8_t hci_le_set_periodic_sync_subevent : 1;
    uint8_t hci_le_extended_create_connection_v2 : 1;
    uint8_t hci_le_set_periodic_advertising_parameters_v2 : 1;
    uint8_t hci_le_read_all_local_supported_features : 1;
    uint8_t hci_le_read_all_remote_features : 1;
    uint8_t hci_le_set_host_feature_v2 : 1;
    uint8_t hci_le_add_device_to_monitored_advertisers_list : 1;
    uint8_t hci_le_remove_device_from_monitored_advertisers_list : 1;
    uint8_t hci_le_clear_monitored_advertisers_list : 1;
    uint8_t hci_le_read_monitored_advertisers_list_size : 1;
    uint8_t hci_le_frame_space_update : 1;
    uint8_t hci_le_set_resolvable_private_address_timeout_v2 : 1;
    uint8_t hci_le_enable_ota_utp_mode : 1;
    uint8_t hci_le_utp_send : 1;
    uint8_t hci_le_connection_rate_request : 1;
    uint8_t hci_le_set_default_rate_parameters : 1;
    uint8_t hci_le_read_minimum_supported_connection_interval : 1;
} sdc_hci_ip_supported_commands_t;

/** @} end of HCI_TYPES */

/**
 * @defgroup HCI_COMMAND_PARAMETERS Parameters
 * @{
 */

/** @brief Read Local Version Information return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t hci_version;
    uint16_t hci_subversion;
    uint8_t lmp_version;
    uint16_t company_identifier;
    uint16_t lmp_subversion;
} sdc_hci_cmd_ip_read_local_version_information_return_t;

/** @brief Read Local Supported Commands return parameter(s). */
typedef union __PACKED __ALIGN(1)
{
    sdc_hci_ip_supported_commands_t params;
    uint8_t raw[64];
} sdc_hci_cmd_ip_read_local_supported_commands_return_t;

/** @brief Read Local Supported Features return parameter(s). */
typedef union __PACKED __ALIGN(1)
{
    sdc_hci_ip_lmp_features_t params;
    uint8_t raw[8];
} sdc_hci_cmd_ip_read_local_supported_features_return_t;

/** @brief Read BD_ADDR return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t bd_addr[6];
} sdc_hci_cmd_ip_read_bd_addr_return_t;

/** @} end of HCI_COMMAND_PARAMETERS */

/**
 * @defgroup HCI_VS_API API
 * @{
 */
/** @brief Read Local Version Information.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.4.1
 *
 * This command reads the values for the version information for the local Controller.
 *
 * The HCI_Version information defines the version information of the HCI layer. The
 * LMP_Version information defines the version of the LMP. The Company_Identifier
 * information indicates the manufacturer of the local device.
 *
 * The HCI_Subversion and LMP_Subversion are vendor-specific.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_Local_Version_Information command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_ip_read_local_version_information(sdc_hci_cmd_ip_read_local_version_information_return_t * p_return);

/** @brief Read Local Supported Commands.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.4.2
 *
 * This command reads the list of HCI commands supported for the local Controller.
 *
 * This command shall return the Supported_Commands configuration parameter.
 *
 * See Section 6.27 for more information.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_Local_Supported_Commands command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_ip_read_local_supported_commands(sdc_hci_cmd_ip_read_local_supported_commands_return_t * p_return);

/** @brief Read Local Supported Features.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.4.3
 *
 * This command requests a list of the supported features for the local BR/EDR Controller.
 * This command will return a list of the LMP features. For details see [Vol 2] Part C,
 * Section 3.2.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_Local_Supported_Features command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_ip_read_local_supported_features(sdc_hci_cmd_ip_read_local_supported_features_return_t * p_return);

/** @brief Read BD_ADDR.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.4.6
 *
 * On a BR/EDR Controller, this command reads the Bluetooth Controller address
 * (BD_ADDR). (See [Vol 2] Part B, Section 1.2 and [Vol 3] Part C, Section 3.2.1).
 *
 * On an LE Controller, this command shall read the Public Device Address as defined in
 * [Vol 6] Part B, Section 1.3. If this Controller does not have a Public Device Address, the
 * value 0x000000000000 shall be returned.
 *
 * On a BR/EDR/LE Controller, the public address shall be the same as the BD_ADDR.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_BD_ADDR command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_ip_read_bd_addr(sdc_hci_cmd_ip_read_bd_addr_return_t * p_return);

/** @} end of HCI_VS_API */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDC_HCI_CMD_INFO_PARAMS_H__ */
