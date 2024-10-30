/*
 * Copyright (c) 2024 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @note this is adapted from https://github.com/AstraeusLabs/web-broadcast-assistant/tree/f1655305f3f1064fde04ed6f6744ca188bdc1670 */

#include <zephyr/types.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/net/buf.h>
#include <zephyr/bluetooth/bluetooth.h>

#include "mock_webusb.h"
#include "broadcast_assistant.h"
#include "message_handler.h"

LOG_MODULE_REGISTER(message_handler, LOG_LEVEL_INF);

NET_BUF_POOL_DEFINE(command_tx_msg_pool, CONFIG_TX_MSG_MAX_MESSAGES, sizeof(struct webusb_message) + CONFIG_TX_MSG_MAX_PAYLOAD_LEN, 0, NULL);

struct webusb_ltv_data {
	uint8_t adv_sid;
	uint16_t pa_interval;
	uint32_t broadcast_id;
	bt_addr_le_t addr;
} __packed;


static void heartbeat_timeout_handler(struct k_timer *dummy_p);
K_TIMER_DEFINE(heartbeat_timer, heartbeat_timeout_handler, NULL);

static void log_ltv(uint8_t *data, uint16_t data_len);

#define LTV_STR_LEN 256

static void log_ltv(uint8_t *data, uint16_t data_len)
{
	char ltv_str[LTV_STR_LEN] = {0};

	/* Log message payload (ltv format) */
	for (int i = 0; i < data_len;) {
		uint8_t ltv_len = *data++;
		char *ch_ptr = &ltv_str[0];

		/* length */
		sprintf(ch_ptr, "[ L:%02x ", ltv_len);
		ch_ptr += 7;
		if (ltv_len > 0) {
			/* type */
			sprintf(ch_ptr, "T:%02x ", *data++);
			ch_ptr += 5;
			if (ltv_len > 1) {
				/* value */
				for (int j = 1; j < ltv_len; j++) {
					sprintf(ch_ptr, "%02x ", *data++);
					ch_ptr += 3;
				}
			}
		}
		sprintf(ch_ptr, "]");
		ch_ptr += 1;
		i += (ltv_len + 1);

		LOG_DBG("%s", ltv_str);
	}
}

static struct webusb_ltv_data parsed_ltv_data;
static void heartbeat_timeout_handler(struct k_timer *timer)
{
	static uint8_t heartbeat_cnt = 0;
	struct net_buf *tx_net_buf;
	int ret;

	tx_net_buf = net_buf_alloc(&command_tx_msg_pool, K_FOREVER);
	if (!tx_net_buf) {
		return;
	}

	net_buf_push_le16(tx_net_buf, 0);
	net_buf_push_u8(tx_net_buf, heartbeat_cnt++);
	net_buf_push_u8(tx_net_buf, MESSAGE_SUBTYPE_HEARTBEAT);
	net_buf_push_u8(tx_net_buf, MESSAGE_TYPE_EVT);

	ret = webusb_transmit(tx_net_buf);
	if (ret != 0) {
		LOG_ERR("Failed to send response (err=%d)", ret);
	}
}

struct net_buf* message_alloc_tx_message(void)
{
	struct net_buf *tx_net_buf;

	tx_net_buf = net_buf_alloc(&command_tx_msg_pool, K_NO_WAIT);
	if (!tx_net_buf) {
		return NULL;
	}

	// Reserve headroom for the webusb msg header
	net_buf_reserve(tx_net_buf, sizeof(struct webusb_message));

	return tx_net_buf;
}

static void send_simple_message(enum message_type mtype, enum message_sub_type stype, uint8_t seq_no, int32_t rc)
{
	struct net_buf *tx_net_buf;
	uint16_t msg_payload_length;
	int ret;

	LOG_DBG("send simple message(%d, %d, %u, %d)", mtype, stype, seq_no, rc);


	tx_net_buf = message_alloc_tx_message();
	if (!tx_net_buf) {
		LOG_ERR("Failed to allocate net_buf");
	}

	/* Append error code payload */
	net_buf_add_u8(tx_net_buf, 5);
	net_buf_add_u8(tx_net_buf, BT_DATA_ERROR_CODE);
	net_buf_add_le32(tx_net_buf, rc);
	msg_payload_length = tx_net_buf->len;

	// Prepend message header
	net_buf_push_le16(tx_net_buf, msg_payload_length);
	net_buf_push_u8(tx_net_buf, seq_no);
	net_buf_push_u8(tx_net_buf, stype);
	net_buf_push_u8(tx_net_buf, mtype);

	log_ltv(&tx_net_buf->data[0], tx_net_buf->len);

	ret = webusb_transmit(tx_net_buf);
	if (ret != 0) {
		LOG_ERR("Failed to send message (err=%d)", ret);
	}

}

void send_response(enum message_sub_type stype, uint8_t seq_no, int32_t rc)
{
	send_simple_message(MESSAGE_TYPE_RES, stype, seq_no, rc);
}

void send_event(enum message_sub_type stype, int32_t rc)
{
	send_simple_message(MESSAGE_TYPE_EVT, stype, 0, rc);
}

void send_net_buf_event(enum message_sub_type stype, struct net_buf *tx_net_buf)
{
	int ret;

	// Prepend message header
	net_buf_push_le16(tx_net_buf, tx_net_buf->len);
	net_buf_push_u8(tx_net_buf, 0);
	net_buf_push_u8(tx_net_buf, stype);
	net_buf_push_u8(tx_net_buf, MESSAGE_TYPE_EVT);

	LOG_DBG("send_net_buf_event(stype: %d)", stype);
	log_ltv(&tx_net_buf->data[0], tx_net_buf->len);

	ret = webusb_transmit(tx_net_buf);
	if (ret != 0) {
		LOG_ERR("Failed to send message (err=%d)", ret);
	}
}

bool ltv_found(struct bt_data *data, void *user_data)
{
	struct webusb_ltv_data *_parsed = (struct webusb_ltv_data *)user_data;

	LOG_DBG("Found LTV structure with type %u", data->type);

	switch (data->type) {
	case BT_DATA_SID:
		LOG_DBG("BT_DATA_SID");
		_parsed->adv_sid = data->data[0];
		return true;
	case BT_DATA_PA_INTERVAL:
		_parsed->pa_interval = sys_get_le16(data->data);
		LOG_DBG("BT_DATA_PA_INTERVAL");
		return true;
	case BT_DATA_BROADCAST_ID:
		_parsed->broadcast_id = sys_get_le24(data->data);
		LOG_DBG("BT_DATA_BROADCAST_ID");
		return true;
	case BT_DATA_RPA:
	case BT_DATA_IDENTITY:
		char addr_str[BT_ADDR_LE_STR_LEN];
		_parsed->addr.type = data->data[0];
		memcpy(&_parsed->addr.a, &data->data[1], sizeof(bt_addr_t));
		bt_addr_le_to_str(&_parsed->addr, addr_str, sizeof(addr_str));
		LOG_DBG("Addr: %s", addr_str);
		return true;
	default:
		LOG_DBG("Unknown type");
	}

	return false;
}

void message_handler(struct webusb_message *msg_ptr, uint16_t msg_length)
{
	if (msg_ptr == NULL) {
		LOG_ERR("Null msg_ptr");
		return;
	}

	/*uint8_t msg_type = msg_ptr->type;*/
	uint8_t msg_sub_type = msg_ptr->sub_type;
	uint8_t msg_seq_no = msg_ptr->seq_no;
	int32_t msg_rc = 0;
	struct net_buf_simple msg_net_buf;

	msg_net_buf.data = msg_ptr->payload;
	msg_net_buf.len = msg_ptr->length;
	msg_net_buf.size = CONFIG_TX_MSG_MAX_PAYLOAD_LEN;
	msg_net_buf.__buf = msg_ptr->payload;

	bt_data_parse(&msg_net_buf, ltv_found, (void *)&parsed_ltv_data);

	switch (msg_sub_type) {
	case MESSAGE_SUBTYPE_HEARTBEAT:
			static bool heartbeat_on = false;
			if (!heartbeat_on) {
			// Start generating heartbeats every second
				heartbeat_on = true;
			k_timer_start(&heartbeat_timer, K_SECONDS(1), K_SECONDS(1));
		} else {
				heartbeat_on = false;
			// Stop heartbeat timer if running
			k_timer_stop(&heartbeat_timer);
		}
		send_response(MESSAGE_SUBTYPE_HEARTBEAT, msg_seq_no, 0);
		break;

	case MESSAGE_SUBTYPE_START_SINK_SCAN:
		LOG_DBG("MESSAGE_SUBTYPE_START_SINK_SCAN");
		msg_rc = start_scan(BROADCAST_ASSISTANT_SCAN_TARGET_SINK);
		send_response(MESSAGE_SUBTYPE_START_SINK_SCAN, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_START_SOURCE_SCAN:
		LOG_DBG("MESSAGE_SUBTYPE_START_SOURCE_SCAN");
		msg_rc = start_scan(BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE);
		send_response(MESSAGE_SUBTYPE_START_SOURCE_SCAN, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_START_SCAN_ALL:
		LOG_DBG("MESSAGE_SUBTYPE_START_SCAN_ALL");
		msg_rc = start_scan(BROADCAST_ASSISTANT_SCAN_TARGET_ALL);
		send_response(MESSAGE_SUBTYPE_START_SCAN_ALL, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_STOP_SCAN:
		LOG_DBG("MESSAGE_SUBTYPE_STOP_SCAN");
		msg_rc = stop_scanning();
		send_response(MESSAGE_SUBTYPE_STOP_SCAN, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_CONNECT_SINK:
		LOG_DBG("MESSAGE_SUBTYPE_CONNECT_SINK (len %u)", msg_length);
		msg_rc = connect_to_sink(&parsed_ltv_data.addr);
		send_response(MESSAGE_SUBTYPE_CONNECT_SINK, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_DISCONNECT_SINK:
		LOG_DBG("MESSAGE_SUBTYPE_DISCONNECT_SINK (len %u)", msg_length);
		msg_rc = disconnect_from_sink(&parsed_ltv_data.addr);
		send_response(MESSAGE_SUBTYPE_DISCONNECT_SINK, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_ADD_SOURCE:
		LOG_DBG("MESSAGE_SUBTYPE_ADD_SOURCE (len %u)", msg_length);
		msg_rc = add_source(parsed_ltv_data.adv_sid, parsed_ltv_data.pa_interval,
				    parsed_ltv_data.broadcast_id, &parsed_ltv_data.addr);
		send_response(MESSAGE_SUBTYPE_ADD_SOURCE, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_REMOVE_SOURCE:
		LOG_DBG("MESSAGE_SUBTYPE_REMOVE_SOURCE (len %u)", msg_length);
		msg_rc = remove_source();
		send_response(MESSAGE_SUBTYPE_REMOVE_SOURCE, msg_seq_no, msg_rc);
		break;

	case MESSAGE_SUBTYPE_RESET:
		LOG_DBG("MESSAGE_SUBTYPE_RESET (len %u)", msg_length);
		msg_rc = stop_scanning();
		send_response(MESSAGE_SUBTYPE_STOP_SCAN, msg_seq_no, msg_rc);
		msg_rc = disconnect_unpair_all();
		send_response(MESSAGE_SUBTYPE_RESET, msg_seq_no, msg_rc);
		// Stop heartbeat if active
		heartbeat_on = false;
		k_timer_stop(&heartbeat_timer);
		break;

	default:
		// Unrecognized message
		send_response(msg_sub_type, msg_seq_no, -1);
		break;
	}
}

void message_handler_init(void)
{
	k_timer_init(&heartbeat_timer, heartbeat_timeout_handler, NULL);
}
