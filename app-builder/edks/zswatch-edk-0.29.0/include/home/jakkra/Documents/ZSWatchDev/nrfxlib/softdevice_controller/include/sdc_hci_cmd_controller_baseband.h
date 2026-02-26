/*
 * Copyright (c) Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sdc_hci_cmd_controller_baseband.h
 *
 * @defgroup sdc_hci_cb SoftDevice Controller HCI Interface for Controller & Baseband commands
 * @ingroup sdc_hci
 *
 * All APIs in this header file are expected to be called from the
 * same execution priority as mpsl_low_priority_process.
 * Not doing so will lead to undefined behavior.
 *
 * @{
 */

#ifndef SDC_HCI_CMD_CONTROLLER_BASEBAND_H__
#define SDC_HCI_CMD_CONTROLLER_BASEBAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nrfx.h>

/**
 * @defgroup HCI_TYPES Types
 * @{
 */

/** @brief HCI CB OpCode Field values. */
enum sdc_hci_opcode_cb
{
    /** @brief See @ref sdc_hci_cmd_cb_set_event_mask(). */
    SDC_HCI_OPCODE_CMD_CB_SET_EVENT_MASK = 0xc01,
    /** @brief See @ref sdc_hci_cmd_cb_reset(). */
    SDC_HCI_OPCODE_CMD_CB_RESET = 0xc03,
    /** @brief See @ref sdc_hci_cmd_cb_read_conn_accept_timeout(). */
    SDC_HCI_OPCODE_CMD_CB_READ_CONN_ACCEPT_TIMEOUT = 0xc15,
    /** @brief See @ref sdc_hci_cmd_cb_write_conn_accept_timeout(). */
    SDC_HCI_OPCODE_CMD_CB_WRITE_CONN_ACCEPT_TIMEOUT = 0xc16,
    /** @brief See @ref sdc_hci_cmd_cb_read_transmit_power_level(). */
    SDC_HCI_OPCODE_CMD_CB_READ_TRANSMIT_POWER_LEVEL = 0xc2d,
    /** @brief See @ref sdc_hci_cmd_cb_set_controller_to_host_flow_control(). */
    SDC_HCI_OPCODE_CMD_CB_SET_CONTROLLER_TO_HOST_FLOW_CONTROL = 0xc31,
    /** @brief See @ref sdc_hci_cmd_cb_host_buffer_size(). */
    SDC_HCI_OPCODE_CMD_CB_HOST_BUFFER_SIZE = 0xc33,
    /** @brief See @ref sdc_hci_cmd_cb_host_number_of_completed_packets(). */
    SDC_HCI_OPCODE_CMD_CB_HOST_NUMBER_OF_COMPLETED_PACKETS = 0xc35,
    /** @brief See @ref sdc_hci_cmd_cb_set_event_mask_page_2(). */
    SDC_HCI_OPCODE_CMD_CB_SET_EVENT_MASK_PAGE_2 = 0xc63,
    /** @brief See @ref sdc_hci_cmd_cb_read_authenticated_payload_timeout(). */
    SDC_HCI_OPCODE_CMD_CB_READ_AUTHENTICATED_PAYLOAD_TIMEOUT = 0xc7b,
    /** @brief See @ref sdc_hci_cmd_cb_write_authenticated_payload_timeout(). */
    SDC_HCI_OPCODE_CMD_CB_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT = 0xc7c,
};

/** @brief Event Mask.
 *
 * If the bit in the Event_Mask is set to a one,
 * then the event associated with that bit will be enabled.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t inquiry_complete_event : 1;
    uint8_t inquiry_result_event : 1;
    uint8_t connection_complete_event : 1;
    uint8_t connection_request_event : 1;
    uint8_t disconnection_complete_event : 1;
    uint8_t authentication_complete_event : 1;
    uint8_t remote_name_request_complete_event : 1;
    uint8_t encryption_change_event_v1 : 1;
    uint8_t change_connection_link_key_complete_event : 1;
    uint8_t link_key_type_changed_event : 1;
    uint8_t read_remote_supported_features_complete_event : 1;
    uint8_t read_remote_version_information_complete_event : 1;
    uint8_t qos_setup_complete_event : 1;
    uint8_t hardware_error_event : 1;
    uint8_t flush_occurred_event : 1;
    uint8_t role_change_event : 1;
    uint8_t mode_change_event : 1;
    uint8_t return_link_keys_event : 1;
    uint8_t pin_code_request_event : 1;
    uint8_t link_key_request_event : 1;
    uint8_t link_key_notification_event : 1;
    uint8_t loopback_command_event : 1;
    uint8_t data_buffer_overflow_event : 1;
    uint8_t max_slots_change_event : 1;
    uint8_t read_clock_offset_complete_event : 1;
    uint8_t connection_packet_type_changed_event : 1;
    uint8_t qos_violation_event : 1;
    uint8_t previously_used30 : 1;
    uint8_t page_scan_repetition_mode_change_event : 1;
    uint8_t flow_specification_complete_event : 1;
    uint8_t inquiry_result_with_rssi_event : 1;
    uint8_t read_remote_extended_features_complete_event : 1;
    uint8_t synchronous_connection_complete_event : 1;
    uint8_t synchronous_connection_changed_event : 1;
    uint8_t sniff_subrating_event : 1;
    uint8_t extended_inquiry_result_event : 1;
    uint8_t encryption_key_refresh_complete_event : 1;
    uint8_t io_capability_request_event : 1;
    uint8_t io_capability_response_event : 1;
    uint8_t user_confirmation_request_event : 1;
    uint8_t user_passkey_request_event : 1;
    uint8_t remote_oob_data_request_event : 1;
    uint8_t simple_pairing_complete_event : 1;
    uint8_t link_supervision_timeout_changed_event : 1;
    uint8_t enhanced_flush_complete_event : 1;
    uint8_t user_passkey_notification_event : 1;
    uint8_t keypress_notification_event : 1;
    uint8_t remote_host_supported_features_notification_event : 1;
    uint8_t le_meta_event : 1;
} sdc_hci_cb_event_mask_t;

/** @brief Event mask page 2.
 *
 * If the bit in the Event_Mask is set to a one,
 * then the event associated with that bit will be enabled.
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t previously_used0 : 1;
    uint8_t previously_used1 : 1;
    uint8_t previously_used2 : 1;
    uint8_t previously_used3 : 1;
    uint8_t previously_used4 : 1;
    uint8_t previously_used5 : 1;
    uint8_t previously_used6 : 1;
    uint8_t previously_used7 : 1;
    uint8_t number_of_completed_data_blocks_event : 1;
    uint8_t previously_used9 : 1;
    uint8_t previously_used10 : 1;
    uint8_t previously_used11 : 1;
    uint8_t previously_used12 : 1;
    uint8_t previously_used13 : 1;
    uint8_t triggered_clock_capture_event : 1;
    uint8_t synchronization_train_complete_event : 1;
    uint8_t synchronization_train_received_event : 1;
    uint8_t connectionless_peripheral_broadcast_receive_event : 1;
    uint8_t connectionless_peripheral_broadcast_timeout_event : 1;
    uint8_t truncated_page_complete_event : 1;
    uint8_t peripheral_page_response_timeout_event : 1;
    uint8_t connectionless_peripheral_broadcast_channel_map_change_event : 1;
    uint8_t inquiry_response_notification_event : 1;
    uint8_t authenticated_payload_timeout_expired_event : 1;
    uint8_t sam_status_change_event : 1;
    uint8_t encryption_change_event_v2 : 1;
} sdc_hci_cb_event_mask_page_2_t;

/** @brief Host Number Of Completed Packets array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t host_num_completed_packets;
} sdc_hci_cb_host_number_of_completed_packets_array_params_t;

/** @} end of HCI_TYPES */

/**
 * @defgroup HCI_COMMAND_PARAMETERS Parameters
 * @{
 */

/** @brief Set Event Mask command parameter(s). */
typedef union __PACKED __ALIGN(1)
{
    sdc_hci_cb_event_mask_t params;
    uint8_t raw[8];
} sdc_hci_cmd_cb_set_event_mask_t;

/** @brief Read Connection Accept Timeout return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_accept_timeout;
} sdc_hci_cmd_cb_read_conn_accept_timeout_return_t;

/** @brief Write Connection Accept Timeout command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_accept_timeout;
} sdc_hci_cmd_cb_write_conn_accept_timeout_t;

/** @brief Read Transmit Power Level command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint8_t type;
} sdc_hci_cmd_cb_read_transmit_power_level_t;

/** @brief Read Transmit Power Level return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    int8_t tx_power_level;
} sdc_hci_cmd_cb_read_transmit_power_level_return_t;

/** @brief Set Controller To Host Flow Control command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t flow_control_enable;
} sdc_hci_cmd_cb_set_controller_to_host_flow_control_t;

/** @brief Host Buffer Size command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t host_acl_data_packet_length;
    uint8_t host_sync_data_packet_length;
    uint16_t host_total_num_acl_data_packets;
    uint16_t host_total_num_sync_data_packets;
} sdc_hci_cmd_cb_host_buffer_size_t;

/** @brief Host Number Of Completed Packets command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t num_handles;
    sdc_hci_cb_host_number_of_completed_packets_array_params_t array_params[];
} sdc_hci_cmd_cb_host_number_of_completed_packets_t;

/** @brief Set Event Mask Page 2 command parameter(s). */
typedef union __PACKED __ALIGN(1)
{
    sdc_hci_cb_event_mask_page_2_t params;
    uint8_t raw[8];
} sdc_hci_cmd_cb_set_event_mask_page_2_t;

/** @brief Read Authenticated Payload Timeout command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_cb_read_authenticated_payload_timeout_t;

/** @brief Read Authenticated Payload Timeout return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t authenticated_payload_timeout;
} sdc_hci_cmd_cb_read_authenticated_payload_timeout_return_t;

/** @brief Write Authenticated Payload Timeout command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t authenticated_payload_timeout;
} sdc_hci_cmd_cb_write_authenticated_payload_timeout_t;

/** @brief Write Authenticated Payload Timeout return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
} sdc_hci_cmd_cb_write_authenticated_payload_timeout_return_t;

/** @} end of HCI_COMMAND_PARAMETERS */

/**
 * @defgroup HCI_VS_API API
 * @{
 */
/** @brief Set Event Mask.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.1
 *
 * This command is used to control which events are generated by the HCI for the Host. If
 * the bit in the Event_Mask is set to a one, then the event associated with that bit will be
 * enabled. For an LE Controller, the “LE Meta event” bit in the event_Mask shall enable or
 * disable all LE events in the LE Meta event (see Section 7.7.65). The event mask allows
 * the Host to control how much it is interrupted.
 *
 * The Controller shall ignore those bits which are reserved for future use or represent
 * events which it does not support. If the Host sets any of these bits to 1, the Controller
 * shall act as if they were set to 0.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Set_Event_Mask command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_set_event_mask(const sdc_hci_cmd_cb_set_event_mask_t * p_params);

/** @brief Reset.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.2
 *
 * This command will reset the Controller and the Link Manager on the BR/EDR Controller
 * or the Link Layer on an LE Controller. If the Controller supports both BR/EDR and LE
 * then the HCI_Reset command shall reset the Link Manager, Baseband and Link Layer.
 * The HCI_Reset command shall not affect the used HCI transport layer since the HCI
 * transport layers may have reset mechanisms of their own. After the reset is completed,
 * the current operational state will be lost, the Controller will enter standby mode and the
 * Controller will automatically revert to the default values for the parameters for which
 * default values are defined in the specification.
 *
 * Note: The HCI_Reset command will not necessarily perform a hardware reset. This is
 * implementation defined.
 *
 * The Host shall not send additional HCI commands before the
 * HCI_Command_Complete event related to the HCI_Reset command has been
 * received.
 *
 * Event(s) generated (unless masked away):
 *
 * When the reset has been performed, an HCI_Command_Complete event shall be
 * generated.
 *
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_reset(void);

/** @brief Read Connection Accept Timeout.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.13
 *
 * This command reads the value for the Connection_Accept_Timeout configuration
 * parameter. See Section 6.7.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_Connection_Timeout command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_read_conn_accept_timeout(sdc_hci_cmd_cb_read_conn_accept_timeout_return_t * p_return);

/** @brief Write Connection Accept Timeout.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.14
 *
 * This command writes the value for the Connection_Accept_Timeout configuration
 * parameter. See Section 6.7.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Write_Connection_Accept_Timeout command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_write_conn_accept_timeout(const sdc_hci_cmd_cb_write_conn_accept_timeout_t * p_params);

/** @brief Read Transmit Power Level.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.35
 *
 * This command reads the values for the TX_Power_Level parameter for the specified
 * Connection_Handle. The Connection_Handle shall be a Connection_Handle for an ACL
 * connection.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_Transmit_Power_Level command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_read_transmit_power_level(const sdc_hci_cmd_cb_read_transmit_power_level_t * p_params,
                                                 sdc_hci_cmd_cb_read_transmit_power_level_return_t * p_return);

/** @brief Set Controller To Host Flow Control.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.38
 *
 * This command is used by the Host to turn flow control on or off for data and/or
 * voice sent in the direction from the Controller to the Host. If flow control is turned off,
 * the Host should not send the HCI_Host_Number_Of_Completed_Packets command.
 * That command will be ignored by the Controller if it is sent by the Host and flow
 * control is off. If flow control is turned on for HCI ACL Data packets and off for HCI
 * Synchronous Data packets, HCI_Host_Number_Of_Completed_Packets commands
 * sent by the Host should only contain Connection_Handles for ACL connections. If
 * flow control is turned off for HCI ACL Data packets and on for HCI Synchronous
 * Data packets, HCI_Host_Number_Of_Completed_Packets commands sent by the Host
 * should only contain Connection_Handles for synchronous connections. If flow control is
 * turned on for HCI ACL Data packets and HCI Synchronous Data packets, the Host will
 * send HCI_Host_Number_Of_Completed_Packets commands both for ACL connections
 * and synchronous connections.
 *
 * The Flow_Control_Enable parameter shall only be changed if no connections exist.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Set_Controller_To_Host_Flow_Control command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_set_controller_to_host_flow_control(const sdc_hci_cmd_cb_set_controller_to_host_flow_control_t * p_params);

/** @brief Host Buffer Size.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.39
 *
 * This command is used by the Host to notify the Controller about the maximum size of
 * the data portion of HCI ACL and Synchronous Data packets sent from the Controller to
 * the Host. The Controller shall fragment the data to be transmitted from the Controller
 * to the Host according to these sizes, so that the HCI Data packets will contain data
 * with up to these sizes. The HCI_Host_Buffer_Size command also notifies the Controller
 * about the total number of HCI ACL and Synchronous Data packets that can be stored
 * in the data buffers of the Host. If flow control from the Controller to the Host is turned
 * off, and the HCI_Host_Buffer_Size command has not been issued by the Host, then this
 * means that the Controller can send HCI Data packets to the Host with any length and
 * it is assumed that the data buffer sizes of the Host are unlimited. If flow control from
 * the Controller to the Host is turned on, the HCI_Host_Buffer_Size command shall after a
 * power-on or a reset always be sent by the Host before the first HCI_Host_Number_Of_-
 * Completed_Packets command is sent.
 *
 * The HCI_Set_Controller_To_Host_Flow_Control command is used to turn flow control
 * on or off. The Host_ACL_Data_Packet_Length parameter will be used to determine the
 * size of the L2CAP fragments contained in ACL Data packets, which are transferred from
 * the Controller to the Host. The Host_Synchronous_Data_Packet_Length parameter is
 * used to determine the maximum size of HCI Synchronous Data packets. Both the Host
 * and the Controller shall support command and event packets, where the data portion
 * (omitting the header) contained in the packets is 255 octets in size.
 *
 * The Host_Total_Num_ACL_Data_Packets parameter contains the total number of HCI
 * ACL Data packets that can be stored in the data buffers of the Host. The Controller will
 * determine how the buffers are to be divided between different Connection_Handles. The
 * Host_Total_Num_Synchronous_Data_Packets parameter gives the same information
 * for HCI Synchronous Data packets. If the Host does not support SCO or eSCO
 * over HCI, then it shall set Host_Total_Num_Synchronous_Data_Packets to zero, in
 * which case the Controller shall ignore the Host_Synchronous_Data_Packet_Length
 * parameter.
 * Note: The Host_ACL_Data_Packet_Length and
 * Host_Synchronous_Data_Packet_Length command parameters do not include the
 * length of the HCI ACL Data packet header or the HCI Synchronous Data packet header
 * respectively.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Host_Buffer_Size command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_host_buffer_size(const sdc_hci_cmd_cb_host_buffer_size_t * p_params);

/** @brief Host Number Of Completed Packets.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.40
 *
 * This command is used by the Host to indicate to the Controller the number
 * of HCI Data packets that have been completed for each Connection_Handle
 * since the previous HCI_Host_Number_Of_Completed_Packets command was
 * sent to the Controller. This means that the corresponding buffer space
 * has been freed in the Host and is available for new packets to be
 * sent. Based on this information, and the Host_Total_Num_ACL_Data_Packets
 * and Host_Total_Num_Synchronous_Data_Packets command parameters of the
 * HCI_Host_Buffer_Size command, the Controller can determine for which
 * Connection_Handles the following HCI Data packets should be sent to the Host.
 * When the Host has completed one or more HCI Data packet(s) it shall send an
 * HCI_Host_Number_Of_Completed_Packets command to the Controller, until it finally
 * reports that all pending HCI Data packets have been completed. The frequency at
 * which this command is sent is manufacturer specific.
 *
 * The HCI_Set_Controller_To_Host_Flow_Control command is used to turn flow control
 * on or off. If flow control from the Controller to the Host is turned on, the
 * HCI_Host_Buffer_Size command shall always be sent by the Host after a power-on
 * or a reset before the first HCI_Host_Number_Of_Completed_Packets command is sent.
 *
 * The HCI_Host_Number_Of_Completed_Packets command may be sent at any time by
 * the Host when there is at least one connection, or if the Controller is in local loopback
 * mode, independent of other commands. If the Host issues this command when neither
 * of these cases applies, the Controller shall ignore it.
 *
 * Event(s) generated (unless masked away):
 *
 * Normally, no event is generated after the HCI_Host_Number_Of_Completed_Packets
 * command has completed. However, if the HCI_Host_Number_Of_Completed_Packets
 * command contains one or more invalid parameters, the Controller shall return an
 * HCI_Command_Complete event containing the error code Invalid HCI Command
 * Parameters (0x12). The normal flow control for commands is not used for this
 * command.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_host_number_of_completed_packets(const sdc_hci_cmd_cb_host_number_of_completed_packets_t * p_params);

/** @brief Set Event Mask Page 2.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.69
 *
 * This command is used to control which events are generated by the HCI for the Host.
 * The Event_Mask_Page_2 is a logical extension to the Event_Mask parameter of the
 * HCI_Set_Event_Mask command. If the bit in the Event_Mask_Page_2 is set to a one,
 * then the event associated with that bit shall be enabled. The event mask allows the
 * Host to control how much it is interrupted.
 *
 * The Controller shall ignore those bits which are reserved for future use or represent
 * events which it does not support. If the Host sets any of these bits to 1, the Controller
 * shall act as if they were set to 0.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Set_Event_Mask_Page_2 command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_set_event_mask_page_2(const sdc_hci_cmd_cb_set_event_mask_page_2_t * p_params);

/** @brief Read Authenticated Payload Timeout.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.93
 *
 * This command reads the Authenticated_Payload_Timeout (authenticatedPayloadTO,
 * see [Vol 2] Part B, Appendix B for BR/EDR connections and [Vol 6] Part B, Section 5.4
 * for LE connections) parameter in the Controller on the specified Connection_Handle.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                           Error code
 * MC     Connection_Handle identifies a BR/EDR synchronous connection or     Command Disallowed
 *        an LE BIS or CIS.                                                   (0x0C)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_Authenticated_Payload_Timeout command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_read_authenticated_payload_timeout(const sdc_hci_cmd_cb_read_authenticated_payload_timeout_t * p_params,
                                                          sdc_hci_cmd_cb_read_authenticated_payload_timeout_return_t * p_return);

/** @brief Write Authenticated Payload Timeout.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.3.94
 *
 * This command writes the Authenticated_Payload_Timeout (authenticatedPayloadTO,
 * see [Vol 2] Part B, Appendix B and [Vol 6] Part B, Section 5.4 for the LE connection)
 * parameter in the Controller for the specified Connection_Handle.
 *
 * When the Connection_Handle identifies a BR/EDR ACL connection:
 *
 * • If the connection is in Sniff mode, the Authenticated_Payload_Timeout shall be equal
 *   to or greater than Tsniff.
 * • If the connection is in Sniff Subrating mode, the Authenticated_Payload_Timeout shall
 *   be equal to or greater than
 *   (max subrate)xTsniff.
 * • If the connection is in Hold mode, the Authenticated_Payload_Timeout shall be equal
 *   to or greater than the holdTO value.
 *
 * When the Connection_Handle identifies an LE ACL connection, the
 * Authenticated_Payload_Timeout shall be equal to or greater than connInterval ×
 * connSubrateFactor × (1 + connPeripheralLatency).
 *
 * When the Connection_Handle is associated with a BR/EDR ACL connection, the Link
 * Manager will use this parameter to determine when to use the LMP ping sequence.
 *
 * When the Connection_Handle is associated with an LE ACL connection, the Link Layer
 * will use this parameter to determine when to use the LE ping sequence.
 *
 * Errors:
 *
 * See Section 4.5.2 for a list of error types and descriptions.
 *
 * <pre>
 * Type   Condition                                                           Error code
 * MC     Connection_Handle identifies a BR/EDR synchronous connection or     Command Disallowed
 *        an LE BIS or CIS.                                                   (0x0C)
 * </pre>
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Write_Authenticated_Payload_Timeout command has completed, an
 * HCI_Command_Complete event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_cb_write_authenticated_payload_timeout(const sdc_hci_cmd_cb_write_authenticated_payload_timeout_t * p_params,
                                                           sdc_hci_cmd_cb_write_authenticated_payload_timeout_return_t * p_return);

/** @} end of HCI_VS_API */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDC_HCI_CMD_CONTROLLER_BASEBAND_H__ */
