/**
 * @file ble_ams.c
 * @author Leonardo Bispo
 *
 * @brief Implements Apple Media Service (AMS), the native iOS GATT server allows the client
 * to control and retrieve media information as artist, etc.
 *
 * RC - Remote Control
 * EU - Entity Update
 * EA - Entity Attribute
 *
 * @see https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleMediaService_Reference/Specification/Specification.html
 */

#include <stdio.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/kernel.h>

#include <bluetooth/gatt_dm.h>
#include <bluetooth/services/ams_client.h>

#include "ble/ble_ams.h"
#include "ble/ble_comm.h"
#include "events/ble_event.h"
#include "events/music_event.h"

LOG_MODULE_REGISTER(ble_ams, CONFIG_ZSW_BLE_LOG_LEVEL);

enum {
    IS_UPDATE_TRACK,
    HAS_NEXT_TRACK,
    HAS_PREVIOUS_TRACK
};

static const enum bt_ams_player_attribute_id entity_update_player[] = {
    BT_AMS_PLAYER_ATTRIBUTE_ID_NAME,
    BT_AMS_PLAYER_ATTRIBUTE_ID_PLAYBACK_INFO,
    BT_AMS_PLAYER_ATTRIBUTE_ID_VOLUME
};

static const enum bt_ams_track_attribute_id entity_update_track[] = {
    BT_AMS_TRACK_ATTRIBUTE_ID_ARTIST,
    BT_AMS_TRACK_ATTRIBUTE_ID_ALBUM,
    BT_AMS_TRACK_ATTRIBUTE_ID_TITLE,
    BT_AMS_TRACK_ATTRIBUTE_ID_DURATION
};

static struct bt_ams_client ams_c;
static char msg_buff[MAX_MUSIC_FIELD_LENGTH + 1];
static struct bt_conn *current_conn;

static void ble_ams_delayed_write_handle(struct k_work *item);
static void ams_discover_retry_handle(struct k_work *item);
static void music_control_event_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);

ZBUS_CHAN_DECLARE(music_control_data_chan);
ZBUS_OBS_DECLARE(ios_music_control_lis);
ZBUS_CHAN_ADD_OBS(music_control_data_chan, ios_music_control_lis, 1);
ZBUS_LISTENER_DEFINE(ios_music_control_lis, music_control_event_callback);

K_WORK_DELAYABLE_DEFINE(ams_gatt_discover_retry, ams_discover_retry_handle);
K_WORK_DELAYABLE_DEFINE(ble_ams_delayed_write, ble_ams_delayed_write_handle);

static void music_control_event_callback(const struct zbus_channel *chan)
{
    const struct music_event *event = zbus_chan_const_msg(chan);

    switch (event->control_type) {
        case MUSIC_CONTROL_UI_PLAY:
            ble_ams_play_pause();
            break;
        case MUSIC_CONTROL_UI_PAUSE:
            ble_ams_play_pause();
            break;
        case MUSIC_CONTROL_UI_NEXT_TRACK:
            ble_ams_next_track();
            break;
        case MUSIC_CONTROL_UI_PREV_TRACK:
            ble_ams_previous_track();
            break;

        case MUSIC_CONTROL_UI_CLOSE:
        default:
            // Nothing to do
            break;
    }
}

static void notify_rc_cb(struct bt_ams_client *ams_c,
                         const uint8_t *data, size_t len)
{
    char str_hex[4];
    bool has_next_track = false;
    bool has_previous_track = false;
    enum bt_ams_remote_command_id cmd_id;

    if (len > 0) {
        /* Each data byte is converted to hexadecimal string, which takes 2 bytes.
         * A comma is added to the hexadecimal except the first data byte.
         * The first byte converted takes 2 bytes buffer and subsequent byte
         * converted takes 3 bytes each.
         */
        if (len * 3 - 1 > MAX_MUSIC_FIELD_LENGTH) {
            LOG_WRN("AMS RC data size is too big\n");
        } else {
            /* Print the accepted Remote Command values. */
            sprintf(msg_buff, "%02X", data[0]);

            for (size_t i = 1; i < len; i++) {
                sprintf(str_hex, ",%02X", data[i]);
                strcat(msg_buff, str_hex);
            }

            LOG_DBG("AMS RC: %s", msg_buff);
        }
    }

    /* Check if track commands are available. */
    for (size_t i = 0; i < len; i++) {
        cmd_id = data[i];
        if (cmd_id == BT_AMS_REMOTE_COMMAND_ID_NEXT_TRACK) {
            has_next_track = true;
        } else if (cmd_id == BT_AMS_REMOTE_COMMAND_ID_PREVIOUS_TRACK) {
            has_previous_track = true;
        }
    }
}

static void notify_eu_cb(struct bt_ams_client *ams_c,
                         const struct bt_ams_entity_update_notif *notif,
                         int err)
{
    uint8_t attr_val;
    char str_hex[9];

    if (!err) {
        switch (notif->ent_attr.entity) {
            case BT_AMS_ENTITY_ID_PLAYER:
                attr_val = notif->ent_attr.attribute.player;
                break;
            case BT_AMS_ENTITY_ID_QUEUE:
                attr_val = notif->ent_attr.attribute.queue;
                break;
            case BT_AMS_ENTITY_ID_TRACK:
                attr_val = notif->ent_attr.attribute.track;
                break;
            default:
                err = -EINVAL;
        }
    }

    if (err) {
        LOG_ERR("AMS EU invalid\n");
    } else if (notif->len > MAX_MUSIC_FIELD_LENGTH) {
        LOG_WRN("AMS EU data size is too big\n");
    } else {
        sprintf(str_hex, "%02X,%02X,%02X",
                notif->ent_attr.entity, attr_val, notif->flags);
        memcpy(msg_buff, notif->data, notif->len);
        msg_buff[notif->len] = '\0';
        LOG_DBG("AMS EU: %s %s", str_hex, msg_buff);

        static struct ble_data_event evt_music_inf = {0};

        if (notif->ent_attr.entity == BT_AMS_ENTITY_ID_TRACK &&
            attr_val == BT_AMS_TRACK_ATTRIBUTE_ID_ARTIST) {
            evt_music_inf.data.type = BLE_COMM_DATA_TYPE_MUSIC_INFO;
            memcpy(&evt_music_inf.data.data.music_info.artist, msg_buff, notif->len);
        }

        if (notif->ent_attr.entity == BT_AMS_ENTITY_ID_TRACK &&
            attr_val == BT_AMS_TRACK_ATTRIBUTE_ID_DURATION) {
            evt_music_inf.data.type = BLE_COMM_DATA_TYPE_MUSIC_INFO;
            // A string containing the floating point value of the total duration of the track in seconds.
            evt_music_inf.data.data.music_info.duration = (int)atof(msg_buff);
        }

        if (notif->ent_attr.entity == BT_AMS_ENTITY_ID_TRACK &&
            attr_val == BT_AMS_TRACK_ATTRIBUTE_ID_TITLE) {
            evt_music_inf.data.type = BLE_COMM_DATA_TYPE_MUSIC_INFO;
            memcpy(&evt_music_inf.data.data.music_info.track_name, msg_buff, notif->len);

            // Only publish when all music information is received, otherwise values are overwritten
            zbus_chan_pub(&ble_comm_data_chan, &evt_music_inf, K_MSEC(250));

            memset(&evt_music_inf, 0, sizeof(evt_music_inf));
        }

        if (notif->ent_attr.entity == BT_AMS_ENTITY_ID_PLAYER &&
            attr_val == BT_AMS_PLAYER_ATTRIBUTE_ID_PLAYBACK_INFO) {

            struct ble_data_event evt_music_state = {0};

            evt_music_state.data.type = BLE_COMM_DATA_TYPE_MUSIC_STATE;

            // A concatenation of three comma-separated values, i.e 0,0.0,0.000
            // where first value is status
            evt_music_state.data.data.music_state.playing = ((msg_buff[0] - '0') == 1) ? true : false;

            // the last is the elapsed time in seconds as double, it sends empty when the phone player is closed
            if (notif->len > sizeof("0,,")) {
                char elapsed_time[sizeof("9999.999")] = {'\0'};
                memcpy(elapsed_time, &msg_buff[6], notif->len - 6);
                evt_music_state.data.data.music_state.position = (int)atof(elapsed_time);
            }

            zbus_chan_pub(&ble_comm_data_chan, &evt_music_state, K_MSEC(250));
        }
    }
}

static void rc_write_cb(struct bt_ams_client *ams_c, uint8_t err)
{
    if (err) {
        LOG_ERR("AMS RC write error 0x%02X", err);
    }
}

static void eu_write_cb(struct bt_ams_client *ams_c, uint8_t err)
{
    if (err) {
        LOG_ERR("AMS EU write error 0x%02X", err);
    }
}

static void enable_notifications(struct bt_ams_client *ams_c)
{
    int err;
    struct bt_ams_entity_attribute_list entity_attribute_list;

    err = bt_ams_subscribe_remote_command(ams_c, notify_rc_cb);
    if (err) {
        LOG_ERR("Cannot subscribe to Remote Command notification (err %d)", err);
    }

    err = bt_ams_subscribe_entity_update(ams_c, notify_eu_cb);
    if (err) {
        LOG_ERR("Cannot subscribe to Entity Update notification (err %d)", err);
    }

    entity_attribute_list.entity = BT_AMS_ENTITY_ID_PLAYER;
    entity_attribute_list.attribute.player = entity_update_player;
    entity_attribute_list.attribute_count = ARRAY_SIZE(entity_update_player);

    err = bt_ams_write_entity_update(ams_c, &entity_attribute_list, eu_write_cb);
    if (err) {
        LOG_ERR("Cannot write to Entity Update (err %d)", err);
    }

    // Enable track information delayed, GATT write fails if too quick
    k_work_schedule(&ble_ams_delayed_write, K_MSEC(250));
}

static void discover_completed_cb(struct bt_gatt_dm *dm, void *ctx)
{
    int err;
    struct bt_ams_client *ams_c = (struct bt_ams_client *)ctx;

    LOG_INF("The discovery procedure succeeded\n");

    bt_gatt_dm_data_print(dm);

    err = bt_ams_handles_assign(dm, ams_c);
    if (err) {
        LOG_ERR("Could not assign AMS client handles, error: %d", err);
    } else {
        enable_notifications(ams_c);
    }

    err = bt_gatt_dm_data_release(dm);
    if (err) {
        LOG_ERR("Could not release the discovery data, error "
                "code: %d",
                err);
    }
}

static void discover_service_not_found_cb(struct bt_conn *conn, void *ctx)
{
    LOG_DBG("The service could not be found during the discovery\n");
}

static void discover_error_found_cb(struct bt_conn *conn, int err, void *ctx)
{
    LOG_ERR("The discovery procedure failed, err %d", err);
}

static const struct bt_gatt_dm_cb discover_cb = {
    .completed = discover_completed_cb,
    .service_not_found = discover_service_not_found_cb,
    .error_found = discover_error_found_cb,
};

static void discover_gattp(struct bt_conn *conn)
{
    int dm_err = bt_gatt_dm_start(conn, BT_UUID_AMS, &discover_cb, &ams_c);
    if (dm_err) {

        // Only one DM discovery can happen at a time, AMS may be running, so queue it
        if (dm_err == -EALREADY) {
            current_conn = conn;
            k_work_schedule(&ams_gatt_discover_retry, K_MSEC(500));
            return;
        }

        LOG_ERR("Failed to start discovery (err %d)", dm_err);
    }
}

static void ams_discover_retry_handle(struct k_work *item)
{
    discover_gattp(current_conn);
}

static void security_changed(struct bt_conn *conn, bt_security_t level,
                             enum bt_security_err err)
{
    if (!err) {
        LOG_DBG("Security changed: level %u", level);

        if (bt_conn_get_security(conn) >= BT_SECURITY_L2) {
            discover_gattp(conn);
        }
    } else {
        LOG_ERR("Security failed: level %u err %d", level, err);
    }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .security_changed = security_changed,
};

static void ble_ams_delayed_write_handle(struct k_work *item)
{
    int err;
    struct bt_ams_entity_attribute_list entity_attribute_list;

    entity_attribute_list.entity = BT_AMS_ENTITY_ID_TRACK;
    entity_attribute_list.attribute.track = entity_update_track;
    entity_attribute_list.attribute_count = ARRAY_SIZE(entity_update_track);

    err = bt_ams_write_entity_update(&ams_c, &entity_attribute_list, eu_write_cb);
    if (err) {
        LOG_ERR("Cannot write to Entity Update (err %d)", err);
    }
}

int ble_ams_init(void)
{
    int err = bt_ams_client_init(&ams_c);

    if (err) {
        LOG_ERR("Failed to start Apple Media Service: 0x%x", err);
        return err;
    }

    LOG_INF("Start Apple Media Service");

    return err;
}

int ble_ams_play_pause()
{
    return bt_ams_write_remote_command(&ams_c,
                                       BT_AMS_REMOTE_COMMAND_ID_TOGGLE_PLAY_PAUSE,
                                       rc_write_cb);
}

int ble_ams_next_track()
{
    return bt_ams_write_remote_command(&ams_c,
                                       BT_AMS_REMOTE_COMMAND_ID_NEXT_TRACK,
                                       rc_write_cb);
}

int ble_ams_previous_track()
{
    return bt_ams_write_remote_command(&ams_c,
                                       BT_AMS_REMOTE_COMMAND_ID_PREVIOUS_TRACK,
                                       rc_write_cb);
}