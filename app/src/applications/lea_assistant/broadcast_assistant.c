/*
 * Copyright (c) 2024 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @note adapted from https://github.com/AstraeusLabs/web-broadcast-assistant
 *  and zephyr/samples/bluetooth/bap_broadcast_assistant
 */

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/bap.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "message_handler.h"
#include "broadcast_assistant.h"

LOG_MODULE_REGISTER(broadcast_assistant, CONFIG_ZSW_LEA_ASSISTANT_APP_LOG_LEVEL);

#define INVALID_BROADCAST_ID           0xFFFFFFFFU
#define PA_SYNC_SKIP                   5
#define PA_SYNC_INTERVAL_TO_TIMEOUT_RATIO 20
#define MAX_NUMBER_OF_SOURCES          20

/* ─── Source tracking ─────────────────────────────────────────────────────── */

typedef struct {
    bt_addr_le_t addr;
    bool base_received;
} source_data_t;

static struct {
    uint8_t num;
    source_data_t data[MAX_NUMBER_OF_SOURCES];
} source_data_list;

static struct k_mutex source_data_list_mutex;

/* ─── BLE state ───────────────────────────────────────────────────────────── */

static struct bt_le_per_adv_sync *pa_sync;
static volatile bool pa_syncing;
static struct k_work pa_sync_delete_work;

static struct bt_conn *ba_sink_conn;
static uint8_t  ba_scan_target;
static uint32_t ba_source_broadcast_id;
static uint8_t  ba_source_id;

/*
 * recv_state: last receive state read from the sink's BASS server.
 * recv_state_valid: set true when recv_state_cb fires during discovery,
 *   meaning the sink already has a receive state occupying a BASS slot.
 *   We must use mod_src instead of add_src to avoid BT_ATT_ERR_WRITE_REQ_REJECTED.
 */
static struct bt_bap_scan_delegator_recv_state recv_state;
static bool recv_state_valid;

/*
 * pending_source_scan: set in discover_cb when recv_state_count > 0 to delay
 * starting the source scan until we have read the existing receive state.
 * Zephyr's BASS discovery subscribes to notifications but does NOT read
 * existing states — we must call bt_bap_broadcast_assistant_read_recv_state()
 * explicitly, then start scanning in recv_state_cb once the read completes.
 */
static bool pending_source_scan;

/*
 * mod_src_intent_start: true  → mod_src was called to *start* PA sync
 *                                (don't remove source after callback)
 *                       false → mod_src was called to *stop* PA sync
 *                                (remove source in callback, existing flow)
 */
static bool mod_src_intent_start;

/*
 * pending_sync_after_clear: when the sink's BASS receive state is stale-SYNCED,
 * the BASS server (see bap_scan_delegator.c:927) skips pa_sync_req_cb entirely
 * (it only runs when state != SYNCED). We must first clear the stale state with
 * mod_src(pa_sync=false), then follow up with mod_src(pa_sync=true) in the
 * callback. pending_pa_interval stores the interval for the follow-up call.
 */
static bool     pending_sync_after_clear;
static uint16_t pending_pa_interval;

/* ─── Forward declarations ───────────────────────────────────────────────── */

static void restart_scanning_if_needed(void);
static bool device_found(struct bt_data *data, void *user_data);
static bool scan_for_source(const struct bt_le_scan_recv_info *info,
                            struct net_buf_simple *ad, scan_recv_data_t *sr_data);
static bool scan_for_sink(const struct bt_le_scan_recv_info *info,
                          struct net_buf_simple *ad, scan_recv_data_t *sr_data);
static void scan_recv_cb(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad);
static void scan_timeout_cb(void);

/* ─── BAP Broadcast Assistant callbacks ──────────────────────────────────── */

static void broadcast_assistant_discover_cb(struct bt_conn *conn, int err,
                                            uint8_t recv_state_count)
{
    const bt_addr_le_t *bt_addr_le;
    char addr_str[BT_ADDR_LE_STR_LEN];
    struct net_buf *evt_msg;

    LOG_INF("BASS discover: err=%d, recv_state_count=%u", err, recv_state_count);

    if (err) {
        LOG_ERR("BASS discover failed, disconnecting");
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        restart_scanning_if_needed();
        return;
    }

    bt_addr_le = bt_conn_get_dst(conn);
    bt_addr_le_to_str(bt_addr_le, addr_str, sizeof(addr_str));
    LOG_INF("Connected to sink %s (recv_states=%u)", addr_str, recv_state_count);

    /* Kick off source scan now that we have a connected sink */
    /*
     * Zephyr's BASS discovery subscribes to notifications but does NOT read
     * existing receive states. If the sink already has a state (e.g. from its
     * autonomous scanning phase), we must read it explicitly so recv_state_cb
     * fires and sets recv_state_valid before we call add_source().
     */
    if (recv_state_count > 0) {
        int read_err = bt_bap_broadcast_assistant_read_recv_state(conn, 0);

        if (read_err) {
            LOG_ERR("read_recv_state(0) failed (%d) — scanning anyway", read_err);
            restart_scanning_if_needed();
        } else {
            /* Scan will start from recv_state_cb once the read completes */
            pending_source_scan = true;
            LOG_INF("Reading existing recv state before scanning");
        }
    } else {
        /* No existing states — start source scan immediately */
        restart_scanning_if_needed();
    }

    /* Notify upper layer */
    evt_msg = message_alloc_tx_message();
    net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
    net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
    net_buf_add_u8(evt_msg, bt_addr_le->type);
    net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
    net_buf_add_u8(evt_msg, 1 + sizeof(int32_t));
    net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
    net_buf_add_le32(evt_msg, 0);
    send_net_buf_event(MESSAGE_SUBTYPE_SINK_CONNECTED, evt_msg);
}

static void broadcast_assistant_recv_state_cb(struct bt_conn *conn, int err,
                                              const struct bt_bap_scan_delegator_recv_state *state)
{
    struct net_buf *evt_msg;
    enum message_sub_type evt_msg_sub_type;
    const bt_addr_le_t *bt_addr_le;

    if (state == NULL) {
        /* NULL state means removed — handled by recv_state_removed_cb */
        if (pending_source_scan) {
            pending_source_scan = false;
            restart_scanning_if_needed();
        }
        return;
    }

    if (err) {
        LOG_ERR("recv_state_cb error %d", err);
        if (pending_source_scan) {
            pending_source_scan = false;
            restart_scanning_if_needed();
        }
        return;
    }

    LOG_INF("recv_state: src_id=%u pa_sync=%u bid=0x%06x",
            state->src_id, state->pa_sync_state, state->broadcast_id);

    /*
     * Always record the latest state. This is critical: if the sink already
     * has a receive state (e.g. from its autonomous scanning phase) with
     * pa_sync_state=NOT_SYNCED(0), we must still detect it so add_source()
     * can call mod_src instead of add_src (avoids BT_ATT_ERR_WRITE_REQ_REJECTED).
     */
    memcpy(&recv_state, state, sizeof(recv_state));
    recv_state_valid = true;
    ba_source_id = state->src_id;

    /* Emit PA state change event to upper layer */
    switch (state->pa_sync_state) {
        case BT_BAP_PA_STATE_NOT_SYNCED:
            LOG_INF("  PA: NOT_SYNCED");
            evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_NOT_SYNCED;
            break;
        case BT_BAP_PA_STATE_INFO_REQ:
            LOG_INF("  PA: INFO_REQ");
            evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_INFO_REQ;
            break;
        case BT_BAP_PA_STATE_SYNCED:
            LOG_INF("  PA: SYNCED (src_id=%u)", state->src_id);
            evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_SYNCED;
            break;
        case BT_BAP_PA_STATE_FAILED:
            LOG_INF("  PA: FAILED");
            evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_FAILED;
            break;
        case BT_BAP_PA_STATE_NO_PAST:
            LOG_INF("  PA: NO_PAST");
            evt_msg_sub_type = MESSAGE_SUBTYPE_NEW_PA_STATE_NO_PAST;
            break;
        default:
            LOG_WRN("  PA: unknown state %u", state->pa_sync_state);
            return;
    }

    bt_addr_le = bt_conn_get_dst(conn);
    evt_msg = message_alloc_tx_message();
    net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
    net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
    net_buf_add_u8(evt_msg, bt_addr_le->type);
    net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
    net_buf_add_u8(evt_msg, 5);
    net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
    net_buf_add_le32(evt_msg, state->broadcast_id);
    send_net_buf_event(evt_msg_sub_type, evt_msg);

    /* If this was the discovery-phase read, start scanning now */
    if (pending_source_scan) {
        pending_source_scan = false;
        LOG_INF("recv_state read complete — starting source scan");
        restart_scanning_if_needed();
    }

    /* BIS sync state changes */
    for (int i = 0; i < state->num_subgroups; i++) {
        if (state->subgroups[i].bis_sync == recv_state.subgroups[i].bis_sync) {
            continue;
        }

        enum message_sub_type bis_type = state->subgroups[i].bis_sync == 0
                                         ? MESSAGE_SUBTYPE_BIS_NOT_SYNCED
                                         : MESSAGE_SUBTYPE_BIS_SYNCED;

        LOG_INF("  BIS[%d]: %s (0x%08x)", i,
                bis_type == MESSAGE_SUBTYPE_BIS_SYNCED ? "SYNCED" : "NOT_SYNCED",
                state->subgroups[i].bis_sync);

        bt_addr_le = bt_conn_get_dst(conn);
        evt_msg = message_alloc_tx_message();
        net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
        net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
        net_buf_add_u8(evt_msg, bt_addr_le->type);
        net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
        net_buf_add_u8(evt_msg, 5);
        net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
        net_buf_add_le32(evt_msg, state->broadcast_id);
        send_net_buf_event(bis_type, evt_msg);
    }
}

static void broadcast_assistant_recv_state_removed_cb(struct bt_conn *conn, uint8_t src_id)
{
    LOG_INF("recv_state removed: src_id=%u", src_id);
    if (src_id == ba_source_id) {
        recv_state_valid = false;
        memset(&recv_state, 0, sizeof(recv_state));
    }
    send_event(MESSAGE_SUBTYPE_SOURCE_REMOVED, 0);
}

static void broadcast_assistant_add_src_cb(struct bt_conn *conn, int err)
{
    const bt_addr_le_t *bt_addr_le = bt_conn_get_dst(ba_sink_conn);
    char addr_str[BT_ADDR_LE_STR_LEN];
    struct net_buf *evt_msg;

    bt_addr_le_to_str(bt_addr_le, addr_str, sizeof(addr_str));

    if (err) {
        LOG_ERR("add_src FAILED (ATT err %d) for %s", err, addr_str);
        /* Notify upper layer with error */
        evt_msg = message_alloc_tx_message();
        net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
        net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
        net_buf_add_u8(evt_msg, bt_addr_le->type);
        net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
        net_buf_add_u8(evt_msg, 5);
        net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
        net_buf_add_le32(evt_msg, ba_source_broadcast_id);
        net_buf_add_u8(evt_msg, 1 + sizeof(int32_t));
        net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
        net_buf_add_le32(evt_msg, err);
        send_net_buf_event(MESSAGE_SUBTYPE_SOURCE_ADDED, evt_msg);
        return;
    }

    LOG_INF("add_src OK for %s — sink should now PA sync", addr_str);

    evt_msg = message_alloc_tx_message();
    net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
    net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
    net_buf_add_u8(evt_msg, bt_addr_le->type);
    net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
    net_buf_add_u8(evt_msg, 5);
    net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
    net_buf_add_le32(evt_msg, ba_source_broadcast_id);
    net_buf_add_u8(evt_msg, 1 + sizeof(int32_t));
    net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
    net_buf_add_le32(evt_msg, 0);
    send_net_buf_event(MESSAGE_SUBTYPE_SOURCE_ADDED, evt_msg);
}

static void broadcast_assistant_mod_src_cb(struct bt_conn *conn, int err)
{
    if (err) {
        LOG_ERR("mod_src FAILED (err %d)", err);
        pending_sync_after_clear = false;
        return;
    }

    if (pending_sync_after_clear) {
        /*
         * Step 1 (clear stale SYNCED) completed. Now send the real sync request.
         * The sink's BASS state is now NOT_SYNCED, so pa_sync_req_cb will fire.
         */
        struct bt_bap_bass_subgroup subgroup = {0};
        struct bt_bap_broadcast_assistant_mod_src_param param = {0};
        int mod_err;

        pending_sync_after_clear = false;
        subgroup.bis_sync  = BT_BAP_BIS_SYNC_NO_PREF;
        param.src_id       = recv_state.src_id;
        param.pa_sync      = true;
        param.pa_interval  = pending_pa_interval;
        param.num_subgroups = 1;
        param.subgroups    = &subgroup;
        mod_src_intent_start = true;

        LOG_INF("mod_src (pa_sync=false) cleared — now sending pa_sync=true (src_id=%u)",
                recv_state.src_id);
        mod_err = bt_bap_broadcast_assistant_mod_src(conn, &param);
        if (mod_err) {
            LOG_ERR("  follow-up mod_src failed (err %d)", mod_err);
        }
        return;
    }

    if (mod_src_intent_start) {
        /* mod_src was used to start PA sync on an existing receive state */
        LOG_INF("mod_src (pa_sync=true) OK — sink should now PA sync");
        return;
    }

    /* mod_src was used to stop sync — now remove the source */
    LOG_INF("mod_src (pa_sync=false) OK — removing source");
    err = bt_bap_broadcast_assistant_rem_src(conn, ba_source_id);
    if (err) {
        LOG_ERR("rem_src failed (err %d)", err);
    }
}

static void broadcast_assistant_rem_src_cb(struct bt_conn *conn, int err)
{
    LOG_INF("rem_src: err=%d", err);
    if (err == 0) {
        recv_state_valid = false;
        memset(&recv_state, 0, sizeof(recv_state));
        ba_source_id = 0;
    }
}

/* ─── BT connection callbacks ────────────────────────────────────────────── */

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (conn != ba_sink_conn) {
        return;
    }

    if (err) {
        const bt_addr_le_t *bt_addr_le = bt_conn_get_dst(conn);
        struct net_buf *evt_msg;

        LOG_ERR("Connection to sink failed (err %d)", err);
        bt_conn_unref(ba_sink_conn);
        ba_sink_conn = NULL;

        evt_msg = message_alloc_tx_message();
        net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
        net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
        net_buf_add_u8(evt_msg, bt_addr_le->type);
        net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
        net_buf_add_u8(evt_msg, 1 + sizeof(int32_t));
        net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
        net_buf_add_le32(evt_msg, err);
        send_net_buf_event(MESSAGE_SUBTYPE_SINK_CONNECTED, evt_msg);

        restart_scanning_if_needed();
        return;
    }

    /* Only set security on our CENTRAL connections (not phone→watch) */
    struct bt_conn_info info;

    bt_conn_get_info(conn, &info);
    if (info.role == BT_CONN_ROLE_CENTRAL) {
        int sec_err = bt_conn_set_security(conn, BT_SECURITY_L2 | BT_SECURITY_FORCE_PAIR);

        if (sec_err) {
            LOG_ERR("Security setup failed (err %d)", sec_err);
        }
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    const bt_addr_le_t *bt_addr_le;
    struct net_buf *evt_msg;

    if (conn != ba_sink_conn) {
        return;
    }

    bt_addr_le = bt_conn_get_dst(conn);
    LOG_INF("Sink disconnected (reason 0x%02x)", reason);

    /* Clear all sink-related state */
    recv_state_valid         = false;
    pending_source_scan      = false;
    pending_sync_after_clear = false;
    memset(&recv_state, 0, sizeof(recv_state));
    ba_source_id = 0;

    bt_conn_unref(ba_sink_conn);
    ba_sink_conn = NULL;

    evt_msg = message_alloc_tx_message();
    net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
    net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
    net_buf_add_u8(evt_msg, bt_addr_le->type);
    net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
    net_buf_add_u8(evt_msg, 1 + sizeof(int32_t));
    net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
    net_buf_add_le32(evt_msg, 0);
    send_net_buf_event(MESSAGE_SUBTYPE_SINK_DISCONNECTED, evt_msg);
}

static void security_changed_cb(struct bt_conn *conn, bt_security_t level,
                                enum bt_security_err err)
{
    /*
     * Guard: this callback fires for ALL connections (phone, sink, etc.).
     * Only trigger BASS discovery for our sink connection.
     */
    if (conn != ba_sink_conn) {
        return;
    }

    LOG_INF("Security: level=%d err=%d", level, err);

    if (err) {
        LOG_ERR("Security failed — disconnecting");
        bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
        return;
    }

    LOG_INF("Starting BASS discovery");
    int disc_err = bt_bap_broadcast_assistant_discover(ba_sink_conn);

    if (disc_err) {
        LOG_ERR("BASS discover start failed (err %d)", disc_err);
        bt_conn_disconnect(ba_sink_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
}

static void identity_resolved_cb(struct bt_conn *conn, const bt_addr_le_t *rpa,
                                 const bt_addr_le_t *identity)
{
    char rpa_str[BT_ADDR_LE_STR_LEN];
    char identity_str[BT_ADDR_LE_STR_LEN];
    struct net_buf *evt_msg;

    bt_addr_le_to_str(rpa, rpa_str, sizeof(rpa_str));
    bt_addr_le_to_str(identity, identity_str, sizeof(identity_str));
    LOG_INF("Identity resolved %s → %s", rpa_str, identity_str);

    evt_msg = message_alloc_tx_message();
    net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
    net_buf_add_u8(evt_msg, BT_DATA_RPA);
    net_buf_add_u8(evt_msg, rpa->type);
    net_buf_add_mem(evt_msg, &rpa->a, sizeof(bt_addr_t));
    net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
    net_buf_add_u8(evt_msg, BT_DATA_IDENTITY);
    net_buf_add_u8(evt_msg, identity->type);
    net_buf_add_mem(evt_msg, &identity->a, sizeof(bt_addr_t));
    send_net_buf_event(MESSAGE_SUBTYPE_IDENTITY_RESOLVED, evt_msg);
}

/* ─── Callback registration ──────────────────────────────────────────────── */

static struct bt_le_scan_cb scan_callbacks = {
    .recv    = scan_recv_cb,
    .timeout = scan_timeout_cb,
};

static struct bt_bap_broadcast_assistant_cb broadcast_assistant_callbacks = {
    .discover           = broadcast_assistant_discover_cb,
    .recv_state         = broadcast_assistant_recv_state_cb,
    .recv_state_removed = broadcast_assistant_recv_state_removed_cb,
    .add_src            = broadcast_assistant_add_src_cb,
    .mod_src            = broadcast_assistant_mod_src_cb,
    .rem_src            = broadcast_assistant_rem_src_cb,
};

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected        = connected,
    .disconnected     = disconnected,
    .security_changed = security_changed_cb,
    .identity_resolved = identity_resolved_cb,
};

/* ─── Source tracking helpers ────────────────────────────────────────────── */

static void source_data_reset(void)
{
    k_mutex_lock(&source_data_list_mutex, K_FOREVER);
    memset(&source_data_list, 0, sizeof(source_data_list));
    k_mutex_unlock(&source_data_list_mutex);
}

static void source_data_add(const bt_addr_le_t *addr)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    k_mutex_lock(&source_data_list_mutex, K_FOREVER);

    for (int i = 0; i < source_data_list.num; i++) {
        if (bt_addr_le_cmp(addr, &source_data_list.data[i].addr) == 0) {
            k_mutex_unlock(&source_data_list_mutex);
            return; /* already tracked */
        }
    }

    if (source_data_list.num < MAX_NUMBER_OF_SOURCES) {
        bt_addr_le_copy(&source_data_list.data[source_data_list.num].addr, addr);
        source_data_list.data[source_data_list.num].base_received = false;
        source_data_list.num++;
        LOG_INF("Source added (%s), (%u)", addr_str, source_data_list.num);
    }

    k_mutex_unlock(&source_data_list_mutex);
}

static bool source_data_get_base_received(const bt_addr_le_t *addr)
{
    bool result = false;

    k_mutex_lock(&source_data_list_mutex, K_FOREVER);
    for (int i = 0; i < source_data_list.num; i++) {
        if (bt_addr_le_cmp(addr, &source_data_list.data[i].addr) == 0) {
            result = source_data_list.data[i].base_received;
            break;
        }
    }
    k_mutex_unlock(&source_data_list_mutex);
    return result;
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

/* ─── PA sync ────────────────────────────────────────────────────────────── */

static void pa_sync_delete(struct k_work *work)
{
    if (pa_sync == NULL) {
        return;
    }
    int err = bt_le_per_adv_sync_delete(pa_sync);

    if (err) {
        LOG_WRN("PA sync delete failed (%d)", err);
    }
    pa_sync = NULL;
    pa_syncing = false;
}

static uint16_t interval_to_sync_timeout(uint16_t pa_interval)
{
    if (pa_interval == BT_BAP_PA_INTERVAL_UNKNOWN) {
        return BT_GAP_PER_ADV_MAX_TIMEOUT;
    }

    uint32_t interval_ms = BT_GAP_PER_ADV_INTERVAL_TO_MS(pa_interval);
    uint32_t timeout = (interval_ms * PA_SYNC_INTERVAL_TO_TIMEOUT_RATIO) / 10;

    return CLAMP(timeout, BT_GAP_PER_ADV_MIN_TIMEOUT, BT_GAP_PER_ADV_MAX_TIMEOUT);
}

static int pa_sync_create(const struct bt_le_scan_recv_info *info)
{
    struct bt_le_per_adv_sync_param param = {0};

    bt_addr_le_copy(&param.addr, info->addr);
    param.options = 0; /* BT_LE_PER_ADV_SYNC_OPT_FILTER_DUPLICATE not supported on nRF */
    param.sid     = info->sid;
    param.skip    = PA_SYNC_SKIP;
    param.timeout = interval_to_sync_timeout(info->interval);

    return bt_le_per_adv_sync_create(&param, &pa_sync);
}

static void pa_synced_cb(struct bt_le_per_adv_sync *sync,
                         struct bt_le_per_adv_sync_synced_info *info)
{
    LOG_DBG("PA synced %p", (void *)sync);
}

static bool base_search(struct bt_data *data, void *user_data)
{
    const struct bt_bap_base *base = bt_bap_base_get_base_from_ad(data);

    if (base == NULL) {
        return true; /* continue parsing */
    }
    *(bool *)user_data = true;
    return false; /* stop parsing */
}

static void pa_recv_cb(struct bt_le_per_adv_sync *sync,
                       const struct bt_le_per_adv_sync_recv_info *info,
                       struct net_buf_simple *buf)
{
    bool base_found = false;

    if (sync != pa_sync) {
        return;
    }

    bt_data_parse(buf, base_search, &base_found);

    if (base_found) {
        struct net_buf *evt_msg;

        LOG_INF("BASE found — deleting PA sync (sink will do direct PA sync)");
        source_data_set_base_received(info->addr);

        k_work_submit(&pa_sync_delete_work);

        evt_msg = message_alloc_tx_message();
        net_buf_add_u8(evt_msg, buf->len + 1);
        net_buf_add_u8(evt_msg, BT_DATA_BASE);
        net_buf_add_mem(evt_msg, buf->data, buf->len);
        net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
        net_buf_add_u8(evt_msg, bt_addr_le_is_identity(info->addr) ? BT_DATA_IDENTITY : BT_DATA_RPA);
        net_buf_add_u8(evt_msg, info->addr->type);
        net_buf_add_mem(evt_msg, &info->addr->a, sizeof(bt_addr_t));
        send_net_buf_event(MESSAGE_SUBTYPE_SOURCE_BASE_FOUND, evt_msg);
    }
}

static void pa_term_cb(struct bt_le_per_adv_sync *sync,
                       const struct bt_le_per_adv_sync_term_info *info)
{
    LOG_DBG("PA terminated %p", (void *)sync);
    if (sync == pa_sync) {
        pa_sync = NULL;
    }
    pa_syncing = false;
}

static struct bt_le_per_adv_sync_cb pa_sync_callbacks = {
    .synced = pa_synced_cb,
    .recv   = pa_recv_cb,
    .term   = pa_term_cb,
};

/* ─── Scan helpers ───────────────────────────────────────────────────────── */

/*
 * With CONFIG_BT_EXT_ADV=y, Zephyr uses LE Set Extended Scan Parameters HCI
 * command automatically, so BT_LE_SCAN_PASSIVE already sees extended advertisers
 * on 1M PHY (AUX_ADV_IND) without any special option.
 * Add BT_LE_SCAN_OPT_CODED to also cover Coded PHY advertisers (nRF5340 Audio DK
 * defaults to 1M, but some configurations use Coded PHY).
 */
static const struct bt_le_scan_param ext_scan_param = {
    .type     = BT_LE_SCAN_TYPE_PASSIVE,
    .options  = BT_LE_SCAN_OPT_CODED,
    .interval = BT_GAP_SCAN_FAST_INTERVAL,
    .window   = BT_GAP_SCAN_FAST_WINDOW,
};

static int do_scan_start(void)
{
    /* Stop any existing scan first — needed to apply extended params */
    int err = bt_le_scan_stop();

    if (err && err != -EALREADY && err != -EOPNOTSUPP) {
        LOG_WRN("Scan stop before restart: %d", err);
    }
    err = bt_le_scan_start(&ext_scan_param, NULL);
    if (err) {
        /* Fall back to legacy scan (best-effort for legacy sinks) */
        LOG_WRN("Extended scan start failed (%d), falling back to legacy", err);
        err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, NULL);
        if (err && err != -EALREADY) {
            LOG_ERR("Scan start failed (err %d)", err);
            return err;
        }
    } else {
        LOG_DBG("Extended scan started");
    }
    return 0;
}

static void restart_scanning_if_needed(void)
{
    if (!ba_scan_target) {
        return;
    }

    LOG_DBG("Restart scan (target=0x%02x)", ba_scan_target);
    int err = do_scan_start();

    if (err) {
        LOG_ERR("Scan restart failed (err %d)", err);
        ba_scan_target = 0;
    }
}

static bool device_found(struct bt_data *data, void *user_data)
{
    scan_recv_data_t *sr = (scan_recv_data_t *)user_data;
    struct bt_uuid_16 adv_uuid;

    switch (data->type) {
        case BT_DATA_NAME_SHORTENED:
        case BT_DATA_NAME_COMPLETE:
            memcpy(sr->bt_name, data->data, MIN(data->data_len, BT_NAME_LEN - 1));
            sr->bt_name_type = (data->type == BT_DATA_NAME_SHORTENED)
                               ? BT_DATA_NAME_SHORTENED : BT_DATA_NAME_COMPLETE;
            break;
        case BT_DATA_BROADCAST_NAME:
            memcpy(sr->broadcast_name, data->data, MIN(data->data_len, BT_NAME_LEN - 1));
            break;
        case BT_DATA_SVC_DATA16:
            if (data->data_len < BT_UUID_SIZE_16 + BT_AUDIO_BROADCAST_ID_SIZE) {
                break;
            }
            if (!bt_uuid_create(&adv_uuid.uuid, data->data, BT_UUID_SIZE_16)) {
                break;
            }
            if (bt_uuid_cmp(&adv_uuid.uuid, BT_UUID_BROADCAST_AUDIO) == 0) {
                sr->broadcast_id = sys_get_le24(data->data + BT_UUID_SIZE_16);
            }
            break;
        case BT_DATA_UUID16_SOME:
        case BT_DATA_UUID16_ALL:
            if (data->data_len % sizeof(uint16_t) != 0U) {
                break;
            }
            for (size_t i = 0; i < data->data_len; i += sizeof(uint16_t)) {
                const struct bt_uuid *uuid;
                uint16_t u16;

                memcpy(&u16, &data->data[i], sizeof(u16));
                uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));
                if (bt_uuid_cmp(uuid, BT_UUID_BASS) == 0) {
                    sr->has_bass = true;
                } else if (bt_uuid_cmp(uuid, BT_UUID_PACS) == 0) {
                    sr->has_pacs = true;
                }
            }
            break;
        default:
            break;
    }

    return true;
}

static bool scan_for_source(const struct bt_le_scan_recv_info *info,
                            struct net_buf_simple *ad, scan_recv_data_t *sr_data)
{
    sr_data->broadcast_id = INVALID_BROADCAST_ID;

    /* Sources are non-connectable periodic advertisers */
    if ((info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) != 0 || info->interval == 0) {
        return false;
    }

    bt_data_parse(ad, device_found, sr_data);

    if (sr_data->broadcast_id == INVALID_BROADCAST_ID) {
        return false;
    }

    LOG_INF("Broadcast Source Found [name, b_name, b_id] = [\"%s\", \"%s\", 0x%06x]",
            sr_data->bt_name, sr_data->broadcast_name, sr_data->broadcast_id);

    source_data_add(info->addr);

    if (!pa_syncing && !source_data_get_base_received(info->addr)) {
        LOG_INF("PA sync create (b_id = 0x%06x)", sr_data->broadcast_id);
        int err = pa_sync_create(info);

        if (err != 0) {
            LOG_WRN("PA sync create failed: %d", err);
        } else {
            pa_syncing = true;
        }
    }

    return true;
}

static bool scan_for_sink(const struct bt_le_scan_recv_info *info,
                          struct net_buf_simple *ad, scan_recv_data_t *sr_data)
{
    /* Sinks are connectable and advertise BASS */
    if ((info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) == 0) {
        return false;
    }

    bt_data_parse(ad, device_found, sr_data);

    if (!sr_data->has_bass) {
        return false;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(info->addr, addr_str, sizeof(addr_str));
    LOG_INF("Broadcast Sink Found: [\"%s\", %s]", sr_data->bt_name, addr_str);
    return true;
}

static void scan_recv_cb(const struct bt_le_scan_recv_info *info, struct net_buf_simple *ad)
{
    struct net_buf_simple ad1, ad2;

    net_buf_simple_clone(ad, &ad1);
    net_buf_simple_clone(ad, &ad2);

    if (ba_scan_target & BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE) {
        scan_recv_data_t sr = {0};

        if (scan_for_source(info, &ad1, &sr)) {
            struct net_buf *evt_msg = message_alloc_tx_message();

            net_buf_add_mem(evt_msg, ad->data, ad->len);
            net_buf_add_u8(evt_msg, 2);
            net_buf_add_u8(evt_msg, BT_DATA_RSSI);
            net_buf_add_u8(evt_msg, info->rssi);
            net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
            net_buf_add_u8(evt_msg, bt_addr_le_is_identity(info->addr) ? BT_DATA_IDENTITY : BT_DATA_RPA);
            net_buf_add_u8(evt_msg, info->addr->type);
            net_buf_add_mem(evt_msg, &info->addr->a, sizeof(bt_addr_t));
            net_buf_add_u8(evt_msg, strlen(sr.bt_name) + 1);
            net_buf_add_u8(evt_msg, sr.bt_name_type);
            net_buf_add_mem(evt_msg, sr.bt_name, strlen(sr.bt_name));
            net_buf_add_u8(evt_msg, 2);
            net_buf_add_u8(evt_msg, BT_DATA_SID);
            net_buf_add_u8(evt_msg, info->sid);
            net_buf_add_u8(evt_msg, 3);
            net_buf_add_u8(evt_msg, BT_DATA_PA_INTERVAL);
            net_buf_add_le16(evt_msg, info->interval);
            net_buf_add_u8(evt_msg, 5);
            net_buf_add_u8(evt_msg, BT_DATA_BROADCAST_ID);
            net_buf_add_le32(evt_msg, sr.broadcast_id);
            send_net_buf_event(MESSAGE_SUBTYPE_SOURCE_FOUND, evt_msg);
        }
    }

    if (ba_scan_target & BROADCAST_ASSISTANT_SCAN_TARGET_SINK) {
        scan_recv_data_t sr = {0};

        if (scan_for_sink(info, &ad2, &sr)) {
            struct net_buf *evt_msg = message_alloc_tx_message();

            net_buf_add_mem(evt_msg, ad->data, ad->len);
            net_buf_add_u8(evt_msg, 2);
            net_buf_add_u8(evt_msg, BT_DATA_RSSI);
            net_buf_add_u8(evt_msg, info->rssi);
            net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
            net_buf_add_u8(evt_msg, bt_addr_le_is_identity(info->addr) ? BT_DATA_IDENTITY : BT_DATA_RPA);
            net_buf_add_u8(evt_msg, info->addr->type);
            net_buf_add_mem(evt_msg, &info->addr->a, sizeof(bt_addr_t));
            net_buf_add_u8(evt_msg, strlen(sr.bt_name) + 1);
            net_buf_add_u8(evt_msg, sr.bt_name_type);
            net_buf_add_mem(evt_msg, sr.bt_name, strlen(sr.bt_name));
            send_net_buf_event(MESSAGE_SUBTYPE_SINK_FOUND, evt_msg);
        }
    }
}

static void scan_timeout_cb(void)
{
    LOG_DBG("Scan timeout");
    ba_scan_target = 0;
    send_event(MESSAGE_SUBTYPE_STOP_SCAN, 0);
}

/* ─── Public API ─────────────────────────────────────────────────────────── */

int start_scan(uint8_t target)
{
    if (target == BROADCAST_ASSISTANT_SCAN_TARGET_ALL ||
        target == BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE) {
        source_data_reset();
    }

    int err = do_scan_start();

    if (err) {
        LOG_ERR("Scan start failed (err %d)", err);
        return err;
    }

    ba_scan_target = target;
    LOG_INF("Scanning started (target: 0x%08x)", ba_scan_target);
    return 0;
}

int stop_scanning(void)
{
    ba_scan_target = 0; /* clear first so callbacks ignore further scan events */

    int err = bt_le_scan_stop();

    if (err && err != -EALREADY) {
        LOG_WRN("Scan stop: %d (tolerated)", err);
    }

    LOG_DBG("Scanning stopped");
    return 0;
}

static void disconnect_one(struct bt_conn *conn, void *data)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    LOG_INF("Disconnecting %s", addr_str);
    if (bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN) != 0) {
        LOG_WRN("Failed to disconnect %s", addr_str);
    }
}

int disconnect_unpair_all(void)
{
    bt_conn_foreach(BT_CONN_TYPE_LE, disconnect_one, NULL);
    int err = bt_unpair(BT_ID_DEFAULT, NULL);

    if (err) {
        LOG_ERR("bt_unpair failed (%d)", err);
    }
    return 0;
}

int connect_to_sink(bt_addr_le_t *bt_addr_le)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    if (ba_sink_conn) {
        return -EAGAIN;
    }

    /* Stop scanning before connecting — but tolerate -EALREADY */
    stop_scanning();

    bt_addr_le_to_str(bt_addr_le, addr_str, sizeof(addr_str));
    LOG_INF("Connecting to %s...", addr_str);

    int err = bt_conn_le_create(bt_addr_le, BT_CONN_LE_CREATE_CONN,
                                BT_LE_CONN_PARAM_DEFAULT, &ba_sink_conn);
    if (err) {
        LOG_ERR("Connection create failed (err %d)", err);
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

    if (!ba_sink_conn) {
        return -ENOTCONN;
    }

    int err = bt_conn_disconnect(ba_sink_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);

    if (err) {
        struct net_buf *evt_msg;

        LOG_ERR("Disconnect failed (err %d)", err);
        evt_msg = message_alloc_tx_message();
        net_buf_add_u8(evt_msg, 1 + BT_ADDR_LE_SIZE);
        net_buf_add_u8(evt_msg, bt_addr_le_is_identity(bt_addr_le) ? BT_DATA_IDENTITY : BT_DATA_RPA);
        net_buf_add_u8(evt_msg, bt_addr_le->type);
        net_buf_add_mem(evt_msg, &bt_addr_le->a, sizeof(bt_addr_t));
        net_buf_add_u8(evt_msg, 1 + sizeof(int32_t));
        net_buf_add_u8(evt_msg, BT_DATA_ERROR_CODE);
        net_buf_add_le32(evt_msg, err);
        send_net_buf_event(MESSAGE_SUBTYPE_SINK_DISCONNECTED, evt_msg);
    }

    return 0;
}

int add_source(uint8_t sid, uint16_t pa_interval, uint32_t broadcast_id, bt_addr_le_t *addr)
{
    if (!ba_sink_conn) {
        LOG_ERR("No sink connected");
        return -ENOTCONN;
    }

    ba_source_broadcast_id = broadcast_id;
    LOG_INF("add_source: sid=%u interval=%u bid=0x%06x", sid, pa_interval, broadcast_id);

    if (recv_state_valid) {
        /*
         * The sink already has a BASS receive state (e.g. from its autonomous
         * scanning phase or a previous session). The slot is occupied and
         * add_src would return BT_ATT_ERR_WRITE_REQ_REJECTED (0xFC/252).
         * Use mod_src to reuse the existing slot.
         *
         * CRITICAL: if the stale state shows pa_sync=SYNCED, the BASS server
         * (bap_scan_delegator.c:927) will skip pa_sync_req_cb entirely — the
         * sink will only get bis_sync_req_cb but has no actual PA sync object.
         * Fix: send mod_src(pa_sync=false) first to clear SYNCED, then send
         * mod_src(pa_sync=true) in the callback (pending_sync_after_clear).
         */
        struct bt_bap_bass_subgroup subgroup = {0};
        struct bt_bap_broadcast_assistant_mod_src_param param = {0};
        int err;

        subgroup.bis_sync  = BT_BAP_BIS_SYNC_NO_PREF;
        param.src_id       = recv_state.src_id;
        param.num_subgroups = 1;
        param.subgroups    = &subgroup;

        if (recv_state.pa_sync_state == BT_BAP_PA_STATE_SYNCED) {
            /* Step 1: clear stale SYNCED state first */
            param.pa_sync     = false;
            param.pa_interval = BT_BAP_PA_INTERVAL_UNKNOWN;
            pending_sync_after_clear = true;
            pending_pa_interval      = pa_interval;
            mod_src_intent_start     = false;
            LOG_INF("  → mod_src(pa_sync=false) to clear stale SYNCED state (src_id=%u)",
                    recv_state.src_id);
        } else {
            /* PA not synced — can go straight to pa_sync=true */
            param.pa_sync      = true;
            param.pa_interval  = pa_interval;
            pending_sync_after_clear = false;
            mod_src_intent_start     = true;
            LOG_INF("  → mod_src(pa_sync=true) existing src_id=%u (was pa_sync=%u)",
                    recv_state.src_id, recv_state.pa_sync_state);
        }

        err = bt_bap_broadcast_assistant_mod_src(ba_sink_conn, &param);
        if (err) {
            LOG_ERR("  mod_src failed (err %d)", err);
            return err;
        }
        return 0;
    }

    /* No existing receive state — add a fresh one */
    struct bt_bap_bass_subgroup subgroup = {0};
    struct bt_bap_broadcast_assistant_add_src_param param = {0};
    int err;

    subgroup.bis_sync = BT_BAP_BIS_SYNC_NO_PREF;
    bt_addr_le_copy(&param.addr, addr);
    param.adv_sid      = sid;
    param.pa_interval  = pa_interval;
    param.broadcast_id = broadcast_id;
    param.pa_sync      = true;
    param.num_subgroups = 1;
    param.subgroups    = &subgroup;

    LOG_INF("  → add_src");
    err = bt_bap_broadcast_assistant_add_src(ba_sink_conn, &param);
    if (err) {
        LOG_ERR("  add_src failed (err %d)", err);
        return err;
    }
    return 0;
}

int remove_source(void)
{
    if (!ba_sink_conn) {
        LOG_ERR("No sink connected");
        return -ENOTCONN;
    }

    struct bt_bap_bass_subgroup subgroup = {0};
    struct bt_bap_broadcast_assistant_mod_src_param param = {0};
    int err;

    param.src_id       = ba_source_id;
    param.pa_sync      = false;
    param.pa_interval  = BT_BAP_PA_INTERVAL_UNKNOWN;
    param.num_subgroups = 1;
    param.subgroups    = &subgroup;
    mod_src_intent_start = false; /* this mod_src is for stopping → rem_src follows */

    LOG_INF("remove_source: mod_src(pa_sync=false) src_id=%u", ba_source_id);
    err = bt_bap_broadcast_assistant_mod_src(ba_sink_conn, &param);
    if (err) {
        LOG_ERR("mod_src failed (err %d)", err);
        return err;
    }
    return 0;
}

int broadcast_assistant_init(void)
{
    ba_sink_conn             = NULL;
    ba_scan_target           = 0;
    recv_state_valid         = false;
    pending_source_scan      = false;
    pending_sync_after_clear = false;
    mod_src_intent_start     = false;
    pa_sync                  = NULL;
    pa_syncing               = false;
    memset(&recv_state, 0, sizeof(recv_state));

    k_work_init(&pa_sync_delete_work, pa_sync_delete);
    k_mutex_init(&source_data_list_mutex);

    bt_le_scan_cb_register(&scan_callbacks);
    bt_le_per_adv_sync_cb_register(&pa_sync_callbacks);
    bt_bap_broadcast_assistant_register_cb(&broadcast_assistant_callbacks);

    LOG_INF("Broadcast assistant initialized");
    return 0;
}
