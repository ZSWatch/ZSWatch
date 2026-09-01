/*
 * Copyright (c) 2024 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @note this is adapted from https://github.com/AstraeusLabs/web-broadcast-assistant/tree/f1655305f3f1064fde04ed6f6744ca188bdc1670 */

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

// #include "webusb.h"
#include "message_handler.h"
#include "broadcast_assistant.h"

LOG_MODULE_REGISTER(broadcast_assistant, CONFIG_ZSW_LEA_ASSISTANT_APP_LOG_LEVEL);

#define INVALID_BROADCAST_ID 0xFFFFFFFFU

#define PA_SYNC_SKIP                      5
#define PA_SYNC_INTERVAL_TO_TIMEOUT_RATIO 20 /* Set the timeout relative to interval */

#define MAX_NUMBER_OF_SOURCES 20

typedef struct source_data {
	bt_addr_le_t addr;
	bool base_received;
} source_data_t;

typedef struct source_data_list {
	uint8_t num;
	source_data_t data[MAX_NUMBER_OF_SOURCES];
} source_data_list_t;

source_data_list_t source_data_list;

static struct k_mutex source_data_list_mutex;
static struct bt_le_per_adv_sync *pa_sync;
static volatile bool pa_syncing;

static struct k_work pa_sync_delete_work;

static void broadcast_assistant_discover_cb(struct bt_conn *conn, int err,
					    uint8_t recv_state_count);
static void broadcast_assistant_recv_state_cb(struct bt_conn *conn, int err,
					      const struct bt_bap_scan_delegator_recv_state *state);
static void broadcast_assistant_recv_state_removed_cb(struct bt_conn *conn,
						      uint8_t src_id);
static void broadcast_assistant_add_src_cb(struct bt_conn *conn, int err);
static void broadcast_assistant_mod_src_cb(struct bt_conn *conn, int err);
static void broadcast_assistant_rem_src_cb(struct bt_conn *conn, int err);
static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);
static void security_changed_cb(struct bt_conn *conn, bt_security_t level,
				enum bt_security_err err);
static void identity_resolved_cb(struct bt_conn *conn,
				 const bt_addr_le_t *rpa,
				 const bt_addr_le_t *identity);
static void restart_scanning_if_needed(void);
static bool device_found(struct bt_data *data, void *user_data);
static bool scan_for_source(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad,
			    scan_recv_data_t *sr_data);
static bool scan_for_sink(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad,
			  scan_recv_data_t *sr_data);
static void scan_recv_cb(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad);
static void scan_timeout_cb(void);

static struct bt_le_scan_cb scan_callbacks = {
	.recv = scan_recv_cb,
	.timeout = scan_timeout_cb,
};

static struct bt_bap_broadcast_assistant_cb broadcast_assistant_callbacks = {
	.discover = broadcast_assistant_discover_cb,
	.recv_state = broadcast_assistant_recv_state_cb,
	.recv_state_removed = broadcast_assistant_recv_state_removed_cb,
	.add_src = broadcast_assistant_add_src_cb,
	.mod_src = broadcast_assistant_mod_src_cb,
	.rem_src = broadcast_assistant_rem_src_cb,
};

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.security_changed = security_changed_cb,
	.identity_resolved = identity_resolved_cb
};

static struct bt_conn *ba_sink_conn; /* TODO: Make a list of sinks */
static uint8_t ba_scan_target;
static uint32_t ba_source_broadcast_id;
static uint8_t ba_source_id; /* Source ID of the receive state */
static struct bt_bap_scan_delegator_recv_state recv_state = {0};

/*
 * Private functions
 */

static void pa_sync_delete(struct k_work *work)
{
	int err;

	LOG_DBG("pa_sync_delete");

	err = bt_le_per_adv_sync_delete(pa_sync);
	if (err) {
		LOG_WRN("bt_le_per_adv_sync_delete failed (%d)", err);
	}
}

static void source_data_reset(void)
{
	k_mutex_lock(&source_data_list_mutex, K_FOREVER);
	for (int i = 0; i < MAX_NUMBER_OF_SOURCES; i++) {
		bt_addr_le_copy(&source_data_list.data[i].addr, BT_ADDR_LE_NONE);
		source_data_list.data[i].base_received = false;

	}
	source_data_list.num = 0;
	k_mutex_unlock(&source_data_list_mutex);
}

static void source_data_add(const bt_addr_le_t *addr)
{
	int i;
	bool new_source = true;
	char addr_str[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

	k_mutex_lock(&source_data_list_mutex, K_FOREVER);
	for (i = 0; i < source_data_list.num; i++) {
		if (bt_addr_le_cmp(addr, &source_data_list.data[i].addr) == 0) {
			LOG_DBG("Source already added (%s)", addr_str);
			new_source = false;
			break;
		}
	}

	if (new_source && i < MAX_NUMBER_OF_SOURCES) {
		bt_addr_le_copy(&source_data_list.data[i].addr, addr);
		source_data_list.data[i].base_received = false;
		source_data_list.num++;
		LOG_INF("Source added (%s), (%u)", addr_str, source_data_list.num);
	}

	k_mutex_unlock(&source_data_list_mutex);
}

static bool source_data_get_base_received(const bt_addr_le_t *addr)
{
	bool base_received = false;

	k_mutex_lock(&source_data_list_mutex, K_FOREVER);
	for (int i = 0; i < source_data_list.num; i++) {
		if (bt_addr_le_cmp(addr, &source_data_list.data[i].addr) == 0 &&
		    source_data_list.data[i].base_received) {
			base_received = true;
			break;
		}
	}
	k_mutex_unlock(&source_data_list_mutex);

	return base_received;
}

static void source_data_set_base_received(const bt_addr_le_t *addr)
{
	k_mutex_lock(&source_data_list_mutex, K_FOREVER);
	for (int i = 0; i < source_data_list.num; i++) {
		if (bt_addr_le_cmp(addr, &source_data_list.data[i].addr) == 0) {
			source_data_list.data[i].base_received = true;
			break;
		}
	}
	k_mutex_unlock(&source_data_list_mutex);
}

static void broadcast_assistant_discover_cb(struct bt_conn *conn, int err, uint8_t recv_state_count)
{
	const bt_addr_le_t *bt_addr_le;
	char addr_str[BT_ADDR_LE_STR_LEN];
	struct net_buf *evt_msg;

	LOG_DBG("Broadcast assistant discover callback (%p, %d, %u)", (void *)conn, err, recv_state_count);
	if (err) {
		err = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
		if (err) {
			LOG_ERR("Failed to disconnect (err %d)", err);
		}
		restart_scanning_if_needed();

		return; /* return and wait for disconnected callback (assume no err) */
	}

	/* Succesful connected to sink */
	evt_msg = message_alloc_tx_message();
	bt_addr_le = bt_conn_get_dst(conn);
	bt_addr_le_to_str(bt_addr_le, addr_str, sizeof(addr_str));
	LOG_DBG("Connected to %s", addr_str);

	/* Bluetooth LE Device Address */
	net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
	net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
	net_buf_add_u8(evt_msg, bt_addr_le->type);
	net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));

	/* error code */
	net_buf_add_u8(evt_msg, 1 /* len of BT_DATA type */ + sizeof(int32_t));
	net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
	net_buf_add_le32(evt_msg, 0 /* OK */);

	restart_scanning_if_needed();
	send_net_buf_event(MESSAGE_SUBTYPE_SINK_CONNECTED, evt_msg); /// @leomod send the message after trying to restart otherwise there is a race-ish condition
}

static void broadcast_assistant_recv_state_cb(struct bt_conn *conn, int err,
			   const struct bt_bap_scan_delegator_recv_state *state)
{
	LOG_DBG("Broadcast assistant recv_state callback (%p, %d)", (void *)conn, err);

	if (state->pa_sync_state != recv_state.pa_sync_state) {
		struct net_buf *evt_msg;
		enum message_sub_type evt_msg_sub_type;
		const bt_addr_le_t *bt_addr_le;

		LOG_DBG("Going from PA state %u to %u", recv_state.pa_sync_state, state->pa_sync_state);

		switch (state->pa_sync_state) {
		case BT_BAP_PA_STATE_NOT_SYNCED:
			LOG_INF("BT_BAP_PA_STATE_NOT_SYNCED");
			evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_NOT_SYNCED;
			break;
		case BT_BAP_PA_STATE_INFO_REQ:
			LOG_INF("BT_BAP_PA_STATE_INFO_REQ");
			evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_INFO_REQ;
			break;
		case BT_BAP_PA_STATE_SYNCED:
			LOG_INF("BT_BAP_PA_STATE_SYNCED (src_id = %u)", state->src_id);
			ba_source_id = state->src_id; /* store source ID of the receive state */
			evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_SYNCED;
			break;
		case BT_BAP_PA_STATE_FAILED:
			LOG_INF("BT_BAP_PA_STATE_FAILED");
			evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_FAILED;
			break;
		case BT_BAP_PA_STATE_NO_PAST:
			LOG_INF("BT_BAP_PA_STATE_NO_PAST");
			evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_NO_PAST;
			break;
		default:
			LOG_INF("Invalid State Transition");
			return;
		}

		evt_msg = message_alloc_tx_message();

		/* Bluetooth LE Device Address */
		bt_addr_le = bt_conn_get_dst(conn);
		net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
		net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
		net_buf_add_u8(evt_msg, bt_addr_le->type);
		net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));

		/* broadcast id */
		net_buf_add_u8(evt_msg, 5);
		net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
		net_buf_add_le32(evt_msg, state->broadcast_id);

		send_net_buf_event(evt_msg_sub_type, evt_msg);
	}

	for (int i = 0; i < state->num_subgroups; i++) {
		if (state->subgroups[i].bis_sync != recv_state.subgroups[i].bis_sync) {
			struct net_buf *evt_msg;
			enum message_sub_type evt_msg_sub_type;
			const bt_addr_le_t *bt_addr_le;

			/* BIS sync changed */
			evt_msg_sub_type = state->subgroups[i].bis_sync == 0
						   ? MESSAGE_SUBTYPE_BIS_NOT_SYNCED
						   : MESSAGE_SUBTYPE_BIS_SYNCED;

			LOG_DBG("%s", evt_msg_sub_type == MESSAGE_SUBTYPE_BIS_SYNCED
					      ? "MESSAGE_SUBTYPE_BIS_SYNCED"
					      : "MESSAGE_SUBTYPE_BIS_NOT_SYNCED");

			evt_msg = message_alloc_tx_message();

			/* Bluetooth LE Device Address */
			bt_addr_le = bt_conn_get_dst(conn);
			net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
			net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
			net_buf_add_u8(evt_msg, bt_addr_le->type);
			net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));

			/* broadcast id */
			net_buf_add_u8(evt_msg, 5);
			net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
			net_buf_add_le32(evt_msg, state->broadcast_id);

			send_net_buf_event(evt_msg_sub_type, evt_msg);
		}
	}

	/* Store latest recv_state */
	memcpy(&recv_state, state, sizeof(struct bt_bap_scan_delegator_recv_state));
}

static void broadcast_assistant_recv_state_removed_cb(struct bt_conn *conn, uint8_t src_id)
{
	LOG_DBG("Broadcast assistant recv_state_removed callback (%p, %u)", (void *)conn, src_id);
	send_event(MESSAGE_SUBTYPE_SOURCE_REMOVED, 0);
}

static void broadcast_assistant_add_src_cb(struct bt_conn *conn, int err)
{
	const bt_addr_le_t *bt_addr_le;
	char addr_str[BT_ADDR_LE_STR_LEN];
	struct net_buf *evt_msg;

	LOG_DBG("Broadcast assistant add_src callback (%p, %d)", (void *)conn, err);

	evt_msg = message_alloc_tx_message();
	bt_addr_le = bt_conn_get_dst(ba_sink_conn); /* sink addr */
	bt_addr_le_to_str(bt_addr_le, addr_str, sizeof(addr_str));
	LOG_DBG("Source added for %s", addr_str);

	/* Bluetooth LE Device Address */
	net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
	net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
	net_buf_add_u8(evt_msg, bt_addr_le->type);
	net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));

	/* broadcast id */
	net_buf_add_u8(evt_msg, 5);
	net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
	net_buf_add_le32(evt_msg, ba_source_broadcast_id);
	/* error code */
	net_buf_add_u8(evt_msg, 1 /* len of BT_DATA type */ + sizeof(int32_t));
	net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
	net_buf_add_le32(evt_msg, err);

	send_net_buf_event(MESSAGE_SUBTYPE_SOURCE_ADDED, evt_msg);
}

static void broadcast_assistant_mod_src_cb(struct bt_conn *conn, int err)
{
	if (err) {
		LOG_ERR("BASS modify source (err: %d)", err);
		return;
	}

	LOG_DBG("BASS modify source (bis_sync = 0, pa_sync = false) ok -> Now remove source");

	err = bt_bap_broadcast_assistant_rem_src(conn, ba_source_id);
	if (err) {
		LOG_ERR("BASS remove source (err: %d)", err);
	}
}

static void broadcast_assistant_rem_src_cb(struct bt_conn *conn, int err)
{
	LOG_INF("BASS remove source (err: %d)", err);
	ba_source_id = 0;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	LOG_DBG("Broadcast assistant connected callback (%p, err:%d)", (void *)conn, err);

	if (conn != ba_sink_conn) {
		return;
	}
	if (err) {
		const bt_addr_le_t *bt_addr_le;
		struct net_buf *evt_msg;

		LOG_ERR("Connected error (err %d)", err);

		evt_msg = message_alloc_tx_message();
		bt_addr_le = bt_conn_get_dst(conn);
		/* Bluetooth LE Device Address */
		net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
		net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
		net_buf_add_u8(evt_msg, bt_addr_le->type);
		net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
		/* error code */
		net_buf_add_u8(evt_msg, 1 /* len of BT_DATA type */ + sizeof(int32_t));
		net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
		net_buf_add_le32(evt_msg, err);

		bt_conn_unref(ba_sink_conn);
		ba_sink_conn = NULL;

		send_net_buf_event(MESSAGE_SUBTYPE_SINK_CONNECTED, evt_msg);
		restart_scanning_if_needed();
		return;
	}

	/// @leomod check for connection role in order to not interfere with phone connection
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
	if(info.role == BT_CONN_ROLE_CENTRAL){
		err = bt_conn_set_security(conn, BT_SECURITY_L2 | BT_SECURITY_FORCE_PAIR);
	}

	if (err) {
		LOG_ERR("Setting security failed (err %d)", err);
	}

}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	const bt_addr_le_t *bt_addr_le;
	struct net_buf *evt_msg;

	LOG_DBG("Broadcast assistant disconnected callback (%p, reason:%d)", (void *)conn, reason);

	if (conn != ba_sink_conn) {
		return;
	}

	bt_addr_le = bt_conn_get_dst(conn);
	evt_msg = message_alloc_tx_message();
	/* Bluetooth LE Device Address */
	net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
	net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
	net_buf_add_u8(evt_msg, bt_addr_le->type);
	net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
	/* error code */
	net_buf_add_u8(evt_msg, 1 /* len of BT_DATA type */ + sizeof(int32_t));
	net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
	net_buf_add_le32(evt_msg, 0 /* OK */);

	bt_conn_unref(ba_sink_conn);
	ba_sink_conn = NULL;

	send_net_buf_event(MESSAGE_SUBTYPE_SINK_DISCONNECTED, evt_msg);
}

static void security_changed_cb(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	LOG_DBG("Broadcast assistant security_changed callback (%p, %d, err:%d)", (void *)conn, level, err);


	/* Connected. Do BAP broadcast assistant discover */
	LOG_INF("Broadcast assistant discover");
	err = bt_bap_broadcast_assistant_discover(ba_sink_conn);
	if (false) { // @leomod do not disconnect
		LOG_ERR("Broadcast assistant discover (err %d)", err);
		err = bt_conn_disconnect(ba_sink_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
		if (err) {
			LOG_ERR("Failed to disconnect (err %d)", err);
		}
		restart_scanning_if_needed();

		return; /* return and wait for disconnected callback (assume no err) */
	}
}

static void identity_resolved_cb(struct bt_conn *conn, const bt_addr_le_t *rpa,
				 const bt_addr_le_t *identity) {
	char rpa_str[BT_ADDR_LE_STR_LEN];
	char identity_str[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(rpa, rpa_str, sizeof(rpa_str));
	bt_addr_le_to_str(identity, identity_str, sizeof(identity_str));
	LOG_INF("Identity resolved %s -> %s", rpa_str, identity_str);

	enum message_sub_type evt_msg_sub_type;
	struct net_buf *evt_msg;

	evt_msg_sub_type = MESSAGE_SUBTYPE_IDENTITY_RESOLVED;
	evt_msg = message_alloc_tx_message();

	net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
	net_buf_add_u8(evt_msg, BT_DATA_RPA);
	net_buf_add_u8(evt_msg, rpa->type);
	net_buf_add_mem(evt_msg, &rpa->a, sizeof(bt_addr_t));

	net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
	net_buf_add_u8(evt_msg, BT_DATA_IDENTITY);
	net_buf_add_u8(evt_msg, identity->type);
	net_buf_add_mem(evt_msg, &identity->a, sizeof(bt_addr_t));

	send_net_buf_event(evt_msg_sub_type, evt_msg);
}

static void restart_scanning_if_needed(void)
{
	int err;

	if (ba_scan_target) {
		LOG_DBG("Restart scanning");
		err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, NULL);
		if (err) {
			LOG_ERR("Scanning failed to start (err %d)", err);
			if (ba_scan_target == BROADCAST_ASSISTANT_SCAN_TARGET_ALL) {
				send_event(MESSAGE_SUBTYPE_START_SCAN_ALL, err);
			} else if (ba_scan_target == BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE) {
				send_event(MESSAGE_SUBTYPE_START_SOURCE_SCAN, err);
			} else if (ba_scan_target == BROADCAST_ASSISTANT_SCAN_TARGET_SINK) {
				send_event(MESSAGE_SUBTYPE_START_SINK_SCAN, err);
			}

			ba_scan_target = 0;
		}
	}
}

static bool device_found(struct bt_data *data, void *user_data)
{
	scan_recv_data_t *sr_data = (scan_recv_data_t *)user_data;
	struct bt_uuid_16 adv_uuid;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
		memcpy(sr_data->bt_name, data->data, MIN(data->data_len, BT_NAME_LEN - 1));
		sr_data->bt_name_type = data->type == BT_DATA_NAME_SHORTENED
						? BT_DATA_NAME_SHORTENED
						: BT_DATA_NAME_COMPLETE;
		return true;
	case BT_DATA_BROADCAST_NAME:
		memcpy(sr_data->broadcast_name, data->data, MIN(data->data_len, BT_NAME_LEN - 1));
		return true;
	case BT_DATA_SVC_DATA16:
		/* TODO: Test code bolow before enable */
#if 0
		/* Check for BASS in Service Data */
		if (data->data_len >= BT_UUID_SIZE_16) {
			const struct bt_uuid *uuid;
			uint16_t u16;

			memcpy(&u16, data->data, sizeof(u16));
			uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));

			if (bt_uuid_cmp(uuid, BT_UUID_BASS)) {
				sr_info->has_bass = true;
				return true;
			}
		}
#endif /* 0 */

		/* Check for Broadcast ID */
		if (data->data_len < BT_UUID_SIZE_16 + BT_AUDIO_BROADCAST_ID_SIZE) {
			return true;
		}

		if (!bt_uuid_create(&adv_uuid.uuid, data->data, BT_UUID_SIZE_16)) {
			return true;
		}

		if (bt_uuid_cmp(&adv_uuid.uuid, BT_UUID_BROADCAST_AUDIO) != 0) {
			return true;
		}

		sr_data->broadcast_id = sys_get_le24(data->data + BT_UUID_SIZE_16);
		return true;
	case BT_DATA_UUID16_SOME:
	case BT_DATA_UUID16_ALL:
		/* NOTE: According to the BAP 1.0.1 Spec,
		 * Section 3.9.2. Additional Broadcast Audio Scan Service requirements,
		 * If the Scan Delegator implements a Broadcast Sink, it should also
		 * advertise a Service Data field containing the Broadcast Audio
		 * Scan Service (BASS) UUID.
		 *
		 * However, it seems that this is not the case with the sinks available
		 * while developing this sample application.  Therefore, we instead,
		 * search for the existence of BASS and PACS in the list of service UUIDs,
		 * which does seem to exist in the sinks available.
		 */

		/* Check for BASS and PACS */
		if (data->data_len % sizeof(uint16_t) != 0U) {
			LOG_ERR("UUID16 AD malformed");
			return true;
		}

		for (size_t i = 0; i < data->data_len; i += sizeof(uint16_t)) {
			const struct bt_uuid *uuid;
			uint16_t u16;

			memcpy(&u16, &data->data[i], sizeof(u16));
			uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));

			if (bt_uuid_cmp(uuid, BT_UUID_BASS) == 0) {
				sr_data->has_bass = true;
				continue;
			}

			if (bt_uuid_cmp(uuid, BT_UUID_PACS) == 0) {
				sr_data->has_pacs = true;
				continue;
			}
		}
		return true;
	default:
		return true;
	}
}

static bool base_search(struct bt_data *data, void *user_data)
{
	const struct bt_bap_base *base = bt_bap_base_get_base_from_ad(data);

	/* Base is NULL if the data does not contain a valid BASE */
	if (base == NULL) {
		return true;
	}

	/* Base found */
	*(bool *)user_data = true;

	return false;
}

static void pa_synced_cb(struct bt_le_per_adv_sync *sync,
			 struct bt_le_per_adv_sync_synced_info *info)
{
	LOG_DBG("PA sync %p synced", (void *)sync);
}

static void pa_recv_cb(struct bt_le_per_adv_sync *sync,
		       const struct bt_le_per_adv_sync_recv_info *info, struct net_buf_simple *buf)
{
	bool base_found = false;

	if (sync != pa_sync) {
		return;
	}

	LOG_DBG("PA receive %p", (void *)sync);
	bt_data_parse(buf, base_search, (void *)&base_found);

	if (base_found) {
		enum message_sub_type evt_msg_sub_type;
		struct net_buf *evt_msg;

		LOG_INF("BASE found, delete PA sync");
		source_data_set_base_received(info->addr);

		k_work_submit(&pa_sync_delete_work);

		evt_msg_sub_type = MESSAGE_SUBTYPE_SOURCE_BASE_FOUND;
		evt_msg = message_alloc_tx_message();

		net_buf_add_u8(evt_msg, buf->len + 1);
		net_buf_add_u8(evt_msg, BT_DATA_BASE);
		net_buf_add_mem(evt_msg, buf->data, buf->len);

		/* Append data from struct bt_le_scan_recv_info (BT addr) */
		/* Bluetooth LE Device Address */
		net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
		net_buf_add_u8(evt_msg, bt_addr_le_is_identity(info->addr) ? BT_DATA_IDENTITY : BT_DATA_RPA);
		net_buf_add_u8(evt_msg, info->addr->type);
		net_buf_add_mem(evt_msg, &info->addr->a, sizeof(bt_addr_t));

		send_net_buf_event(evt_msg_sub_type, evt_msg);
	}
}

static void pa_term_cb(struct bt_le_per_adv_sync *sync,
		       const struct bt_le_per_adv_sync_term_info *info)
{
	LOG_DBG("PA terminated %p", (void *)sync);
	pa_syncing = false;
}

static struct bt_le_per_adv_sync_cb pa_synced_callbacks = {
	.synced = pa_synced_cb,
	.recv = pa_recv_cb,
	.term = pa_term_cb,
};

static uint16_t interval_to_sync_timeout(uint16_t pa_interval)
{
	uint16_t pa_timeout;

	if (pa_interval == BT_BAP_PA_INTERVAL_UNKNOWN) {
		/* Use maximum value to maximize chance of success */
		pa_timeout = BT_GAP_PER_ADV_MAX_TIMEOUT;
	} else {
		uint32_t interval_ms;
		uint32_t timeout;

		/* Add retries and convert to unit in 10's of ms */
		interval_ms = BT_GAP_PER_ADV_INTERVAL_TO_MS(pa_interval);
		timeout = (interval_ms * PA_SYNC_INTERVAL_TO_TIMEOUT_RATIO) / 10;

		/* Enforce restraints */
		pa_timeout = CLAMP(timeout, BT_GAP_PER_ADV_MIN_TIMEOUT, BT_GAP_PER_ADV_MAX_TIMEOUT);
	}

	return pa_timeout;
}

static int pa_sync_create(const struct bt_le_scan_recv_info *info)
{
	struct bt_le_per_adv_sync_param per_adv_sync_param = {0};

	bt_addr_le_copy(&per_adv_sync_param.addr, info->addr);
	per_adv_sync_param.options = BT_LE_PER_ADV_SYNC_OPT_FILTER_DUPLICATE;
	per_adv_sync_param.sid = info->sid;
	per_adv_sync_param.skip = PA_SYNC_SKIP;
	per_adv_sync_param.timeout = interval_to_sync_timeout(info->interval);

	return bt_le_per_adv_sync_create(&per_adv_sync_param, &pa_sync);
}

static bool scan_for_source(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad,
			    scan_recv_data_t *sr_data)
{
	/* Scan for and select Broadcast Source */

	sr_data->broadcast_id = INVALID_BROADCAST_ID;

	/* We are only interested in non-connectable periodic advertisers */
	if ((info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) != 0 || info->interval == 0) {
		return false;
	}

	bt_data_parse(ad, device_found, (void *)sr_data);

	if (sr_data->broadcast_id != INVALID_BROADCAST_ID) {
		LOG_INF("Broadcast Source Found [name, b_name, b_id] = [\"%s\", \"%s\", 0x%06x]",
			sr_data->bt_name, sr_data->broadcast_name, sr_data->broadcast_id);

		source_data_add(info->addr);

		if (!pa_syncing && !source_data_get_base_received(info->addr)) {
			LOG_INF("PA sync create (b_id = 0x%06x)", sr_data->broadcast_id);
			int err = pa_sync_create(info);
			if (err != 0) {
				LOG_WRN("Could not create Broadcast PA sync: %d", err);
			} else {
				pa_syncing = true;
			}
		}

		return true;
	}

	return false;
}

static bool scan_for_sink(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad,
			  scan_recv_data_t *sr_data)
{
	/* Scan for and connect to Broadcast Sink */

	/* We are only interested in connectable advertisers */
	if ((info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) == 0) {
		return false;
	}

	bt_data_parse(ad, device_found, (void *)sr_data);

	if (sr_data->has_bass) {
		char addr_str[BT_ADDR_LE_STR_LEN];

		bt_addr_le_to_str(info->addr, addr_str, sizeof(addr_str));
		LOG_INF("Broadcast Sink Found: [\"%s\", %s]", sr_data->bt_name, addr_str);

		return true;
	}

	return false;
}

static void scan_recv_cb(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad)
{
	struct net_buf_simple ad_clone1, ad_clone2;

	/* Clone needed for the event message because bt_data_parse consumes ad data */
	net_buf_simple_clone(ad, &ad_clone1);
	net_buf_simple_clone(ad, &ad_clone2);

	if (ba_scan_target & BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE) {
		enum message_sub_type evt_msg_sub_type;
		struct net_buf *evt_msg;
		scan_recv_data_t sr_data = {0};

		if (scan_for_source(info, &ad_clone1, &sr_data)) {
			/* broadcast source found */
			evt_msg_sub_type = MESSAGE_SUBTYPE_SOURCE_FOUND;
			evt_msg = message_alloc_tx_message();

			net_buf_add_mem(evt_msg, ad->data, ad->len);

			/* Append data from struct bt_le_scan_recv_info (RSSI, BT addr, ..) */
			/* RSSI */
			net_buf_add_u8(evt_msg, 2);
			net_buf_add_u8(evt_msg, BT_DATA_RSSI);
			net_buf_add_u8(evt_msg, info->rssi);
			/* Bluetooth LE Device Address */
			net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
			net_buf_add_u8(evt_msg, bt_addr_le_is_identity(info->addr) ? BT_DATA_IDENTITY : BT_DATA_RPA);
			net_buf_add_u8(evt_msg, info->addr->type);
			net_buf_add_mem(evt_msg, &info->addr->a, sizeof(bt_addr_t));
			/* BT name */
			net_buf_add_u8(evt_msg, strlen(sr_data.bt_name) + 1);
			net_buf_add_u8(evt_msg, sr_data.bt_name_type);
			net_buf_add_mem(evt_msg, &sr_data.bt_name, strlen(sr_data.bt_name));

			/* sid */
			net_buf_add_u8(evt_msg, 2);
			net_buf_add_u8(evt_msg, BT_DATA_SID);
			net_buf_add_u8(evt_msg, info->sid);
			/* pa interval */
			net_buf_add_u8(evt_msg, 3);
			net_buf_add_u8(evt_msg, BT_DATA_PA_INTERVAL);
			net_buf_add_le16(evt_msg, info->interval);
			/* broadcast id */
			net_buf_add_u8(evt_msg, 5);
			net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
			net_buf_add_le32(evt_msg, sr_data.broadcast_id);

			send_net_buf_event(evt_msg_sub_type, evt_msg);
		}
	}

	if (ba_scan_target & BROADCAST_ASSISTANT_SCAN_TARGET_SINK) {
		enum message_sub_type evt_msg_sub_type;
		struct net_buf *evt_msg;
		scan_recv_data_t sr_data = {0};

		if (scan_for_sink(info, &ad_clone2, &sr_data)) {
			/* broadcast sink found */
			evt_msg_sub_type = MESSAGE_SUBTYPE_SINK_FOUND;
			evt_msg = message_alloc_tx_message();

			net_buf_add_mem(evt_msg, ad->data, ad->len);

			/* Append data from struct bt_le_scan_recv_info (RSSI, BT addr, ..) */
			/* RSSI */
			net_buf_add_u8(evt_msg, 2);
			net_buf_add_u8(evt_msg, BT_DATA_RSSI);
			net_buf_add_u8(evt_msg, info->rssi);
			/* Bluetooth LE Device Address */
			net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
			net_buf_add_u8(evt_msg, bt_addr_le_is_identity(info->addr) ? BT_DATA_IDENTITY : BT_DATA_RPA);
			net_buf_add_u8(evt_msg, info->addr->type);
			net_buf_add_mem(evt_msg, &info->addr->a, sizeof(bt_addr_t));
			/* BT name */
			net_buf_add_u8(evt_msg, strlen(sr_data.bt_name) + 1);
			net_buf_add_u8(evt_msg, sr_data.bt_name_type);
			net_buf_add_mem(evt_msg, &sr_data.bt_name, strlen(sr_data.bt_name));

			send_net_buf_event(evt_msg_sub_type, evt_msg);
		}
	}
}

static void scan_timeout_cb(void)
{
	LOG_DBG("Scan timeout");

	ba_scan_target = 0;

	send_event(MESSAGE_SUBTYPE_STOP_SCAN, 0);
}

/*
 * Public functions
 */

int start_scan(uint8_t target)
{
	if (ba_scan_target != 0) {
		/* Scan already ongoing */
		return 0;
	}

	if (target == BROADCAST_ASSISTANT_SCAN_TARGET_ALL ||
	    target == BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE) {
		source_data_reset();
	}

	int err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, NULL);
	if (err) {
		LOG_ERR("Scanning failed to start (err %d)", err);
		return err;
	}

	ba_scan_target = target;

	LOG_INF("Scanning started (target: 0x%08x)", ba_scan_target);

	return 0;
}

int stop_scanning(void)
{
	if (ba_scan_target == 0) {
		/* No scan ongoing */
		return 0;
	}

	int err = bt_le_scan_stop();
	if (err) {
		LOG_ERR("bt_le_scan_stop failed with %d", err);
		return err;
	}

	ba_scan_target = 0;

	LOG_DBG("Scanning stopped");

	return 0;
}

static void disconnect(struct bt_conn *conn, void *data)
{
	char addr_str[BT_ADDR_LE_STR_LEN];
	int err;

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));

	LOG_INF("Disconnecting from %s", addr_str);
	err = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
	if (err) {
		LOG_WRN("Failed to disconnect from %s", addr_str);
	}
}

int disconnect_unpair_all(void)
{
	int err = 0;

	LOG_DBG("Disconnecting and unpairing all devices");

	bt_conn_foreach(BT_CONN_TYPE_LE, disconnect, NULL);

	LOG_DBG("Disconnecting complete");

	err = bt_unpair(BT_ID_DEFAULT, NULL);
	if (err) {
		LOG_ERR("bt_unpair failed with %d", err);
	}

	LOG_DBG("Unpair complete");

	return 0;
}

int connect_to_sink(bt_addr_le_t *bt_addr_le)
{
	char addr_str[BT_ADDR_LE_STR_LEN];
	int err;

	if (ba_sink_conn) {
		/* Sink already connected. TODO: Support multiple sinks*/
		return -EAGAIN;
	}

	/* Stop scanning if needed */
	if (ba_scan_target) {
		LOG_DBG("Stop scanning");
		/// @leomod
		stop_scanning();
	}

	bt_addr_le_to_str(bt_addr_le, addr_str, sizeof(addr_str));
	LOG_INF("Connecting to %s...", addr_str);

	err = bt_conn_le_create(bt_addr_le, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT,
				&ba_sink_conn);
	if (err) {
		LOG_ERR("Failed creating connection (err=%d)", err);
		restart_scanning_if_needed();

		return err;
	}

	return 0;
}

int disconnect_from_sink(bt_addr_le_t *bt_addr_le)
{
	char addr_str[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_addr_le, addr_str, sizeof(addr_str));
	LOG_INF("Disconnecting from %s...", addr_str);

	/* TODO: Support multiple sinks. Search for conn to disconnect via
	 * bt_conn_foreach(BT_CONN_TYPE_LE, disconnect, NULL).
	 */
	if (ba_sink_conn) {
		int err;

		err = bt_conn_disconnect(ba_sink_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
		if (err) {
			struct net_buf *evt_msg;

			LOG_ERR("Failed to disconnect (err %d)", err);
			evt_msg = message_alloc_tx_message();
			/* Bluetooth LE Device Address */
			net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
			net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
			net_buf_add_u8(evt_msg, bt_addr_le->type);
			net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
			/* error code */
			net_buf_add_u8(evt_msg, 1 /* len of BT_DATA type */ + sizeof(int32_t));
			net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
			net_buf_add_le32(evt_msg, err);

			send_net_buf_event(MESSAGE_SUBTYPE_SINK_DISCONNECTED, evt_msg);
		}
	}

	return 0;
}

int add_source(uint8_t sid, uint16_t pa_interval, uint32_t broadcast_id, bt_addr_le_t *addr)
{
	LOG_INF("Adding broadcast source...");

	struct bt_bap_bass_subgroup subgroup = {0};
	struct bt_bap_broadcast_assistant_add_src_param param = {0};
	int err = 0;

	subgroup.bis_sync = BT_BAP_BIS_SYNC_NO_PREF; /* We might want to hard code to BIT(1) */

	bt_addr_le_copy(&param.addr, addr);
	param.adv_sid = sid;
	param.pa_interval = pa_interval;
	param.broadcast_id = broadcast_id;
	param.pa_sync = true;

	/* keep broadcast_id as global variable */
	ba_source_broadcast_id = broadcast_id;

	LOG_INF("adv_sid = %u, pa_interval = %u, broadcast_id = 0x%08x", param.adv_sid,
		param.pa_interval, param.broadcast_id);

	param.num_subgroups = 1;
	param.subgroups = &subgroup;

	if (!ba_sink_conn) {
		LOG_INF("No sink connected!");
		return -ENOTCONN;
	}

	err = bt_bap_broadcast_assistant_add_src(ba_sink_conn, &param);
	if (err) {
		LOG_ERR("Failed to add source (err %d)", err);
		return err;
	}

	return 0;
}

int remove_source(void)
{
	LOG_INF("Removing broadcast source...");

	struct bt_bap_bass_subgroup subgroup = {0}; /* bis_sync = 0 */
	struct bt_bap_broadcast_assistant_mod_src_param param = { 0 };
	int err = 0;

	param.src_id = ba_source_id;
	param.pa_sync = false; /* stop sync to periodic advertisements */
	param.pa_interval = BT_BAP_PA_INTERVAL_UNKNOWN;
	param.num_subgroups = 1; /* TODO: Support multiple subgroups */
	param.subgroups = &subgroup;

	if (!ba_sink_conn) {
		LOG_INF("No sink connected!");
		return -ENOTCONN;
	}

	err = bt_bap_broadcast_assistant_mod_src(ba_sink_conn, &param);
	if (err) {
		LOG_ERR("Failed to modify source (err %d)", err);
		return err;
	}

	return 0;
}

int broadcast_assistant_init(void)
{
	ba_sink_conn = NULL;

	k_work_init(&pa_sync_delete_work, &pa_sync_delete);

	int err = 0; // bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return err;
	}

	LOG_DBG("Bluetooth initialized");

	bt_le_scan_cb_register(&scan_callbacks);
	bt_le_per_adv_sync_cb_register(&pa_synced_callbacks);
	bt_bap_broadcast_assistant_register_cb(&broadcast_assistant_callbacks);
	LOG_DBG("Bluetooth scan callback registered");

	k_mutex_init(&source_data_list_mutex);
	ba_scan_target = 0;

	return 0;
}
