/**
 * @file ble_ancs.c
 * @author Leonardo Bispo
 *
 * @brief Implements Apple Notification Center Service (ANCS), the native iOS GATT server allows the client
 * to receive notifications and retrieve information from Apps.
 *
 * NC - Notification Source
 * CP - Control Point
 * DS - Data Source
 *
 * @see https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Specification/Specification.html
 */
#include "string.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/zbus/zbus.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <bluetooth/gatt_dm.h>
#include <bluetooth/services/ancs_client.h>
#include <bluetooth/services/gattp.h>

#include <zephyr/settings/settings.h>

#include "ble/ble_comm.h"
#include "events/ble_event.h"

LOG_MODULE_REGISTER(ble_ancs, CONFIG_ZSW_BLE_LOG_LEVEL);

enum {
    DISCOVERY_ANCS_ONGOING,
    DISCOVERY_ANCS_SUCCEEDED,
    SERVICE_CHANGED_INDICATED
};

static struct bt_ancs_client ancs_c;

static struct bt_gattp gattp;

static atomic_t discovery_flags;

/* Local copy to keep track of the newest arriving notifications. */
static struct bt_ancs_evt_notif notification_latest;
/* Local copy of the newest notification attribute. */
static struct bt_ancs_attr notif_attr_latest;
/* Local copy of the newest app attribute. */
static struct bt_ancs_attr notif_attr_app_id_latest;
/* Local copy of the current connection. */
static struct bt_conn *current_conn;
/* Buffers to store attribute data. */
static uint8_t attr_appid[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_title[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_subtitle[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_message[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_message_size[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_date[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_posaction[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_negaction[BT_ANCS_ATTR_DATA_MAX];
static uint8_t attr_disp_name[BT_ANCS_ATTR_DATA_MAX];

/* String literals for the iOS notification attribute types.
 * Used when printing to UART.
 */
static const char *lit_attrid[BT_ANCS_NOTIF_ATTR_COUNT] = {
    "App Identifier",
    "Title",
    "Subtitle",
    "Message",
    "Message Size",
    "Date",
    "Positive Action Label",
    "Negative Action Label"
};

enum {
    ATTR_ID_APP_ID = 0,
    ATTR_ID_TITLE,
    ATTR_ID_SUBTITLE,
    ATTR_ID_MESSAGE,
    ATTR_ID_MESSAGE_SIZE,
    ATTR_ID_DATE,
    ATTR_ID_POSITIVE_ACTION_LABEL,
    ATTR_ID_NEGATIVE_ACTION_LABEL,
};

/* String literals for the iOS notification attribute types.
 * Used When printing to UART.
 */
static const char *lit_appid[BT_ANCS_APP_ATTR_COUNT] = {"Display Name"};

static void discover_ancs_first(struct bt_conn *conn);
static void discover_ancs_again(struct bt_conn *conn);
static void bt_ancs_notification_source_handler(struct bt_ancs_client *ancs_c, int err,
                                                const struct bt_ancs_evt_notif *notif);
static void bt_ancs_data_source_handler(struct bt_ancs_client *ancs_c, const struct bt_ancs_attr_response *response);
static void bt_ancs_write_response_handler(struct bt_ancs_client *ancs_c, uint8_t err);
static void gatt_discover_retry_handle(struct k_work *item);

K_WORK_DELAYABLE_DEFINE(gatt_discover_retry, gatt_discover_retry_handle);
ZBUS_CHAN_DECLARE(ble_comm_data_chan);

static void enable_ancs_notifications(struct bt_ancs_client *ancs_c)
{
    int err;

    err = bt_ancs_subscribe_notification_source(ancs_c, bt_ancs_notification_source_handler);
    if (err) {
        LOG_ERR("Failed to enable Notification Source notification (err %d)", err);
    }

    err = bt_ancs_subscribe_data_source(ancs_c, bt_ancs_data_source_handler);
    if (err) {
        LOG_ERR("Failed to enable Data Source notification (err %d)", err);
    }
}

static void discover_ancs_completed_cb(struct bt_gatt_dm *dm, void *ctx)
{
    struct bt_ancs_client *ancs_c = (struct bt_ancs_client *)ctx;
    struct bt_conn *conn = bt_gatt_dm_conn_get(dm);

    LOG_INF("The discovery procedure for ANCS succeeded");

    bt_gatt_dm_data_print(dm);

    int err = bt_ancs_handles_assign(dm, ancs_c);
    if (err) {
        LOG_DBG("Could not init ANCS client object, error: %d", err);
    } else {
        atomic_set_bit(&discovery_flags, DISCOVERY_ANCS_SUCCEEDED);
        enable_ancs_notifications(ancs_c);
    }

    err = bt_gatt_dm_data_release(dm);
    if (err) {
        LOG_ERR("Could not release the discovery data, error code: %d", err);
    }

    atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_ONGOING);
    discover_ancs_again(conn);
}

static void discover_ancs_not_found_cb(struct bt_conn *conn, void *ctx)
{
    LOG_DBG("ANCS could not be found during the discovery");

    atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_ONGOING);
    discover_ancs_again(conn);
}

static void discover_ancs_error_found_cb(struct bt_conn *conn, int err, void *ctx)
{
    LOG_WRN("The discovery procedure for ANCS failed, err %d", err);

    atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_ONGOING);
    discover_ancs_again(conn);
}

static const struct bt_gatt_dm_cb discover_ancs_cb = {
    .completed = discover_ancs_completed_cb,
    .service_not_found = discover_ancs_not_found_cb,
    .error_found = discover_ancs_error_found_cb,
};

static void indicate_sc_cb(struct bt_gattp *gattp,
                           const struct bt_gattp_handle_range *handle_range,
                           int err)
{
    if (!err) {
        atomic_set_bit(&discovery_flags, SERVICE_CHANGED_INDICATED);
        discover_ancs_again(gattp->conn);
    }
}

static void enable_gattp_indications(struct bt_gattp *gattp)
{
    int err = bt_gattp_subscribe_service_changed(gattp, indicate_sc_cb);
    if (err) {
        LOG_ERR("Cannot subscribe to Service Changed indication (err %d)", err);
    }
}

static void discover_gattp_completed_cb(struct bt_gatt_dm *dm, void *ctx)
{
    int err;
    struct bt_gattp *gattp = (struct bt_gattp *)ctx;
    struct bt_conn *conn = bt_gatt_dm_conn_get(dm);

    /* Checks if the service is empty.
     * Discovery Manager handles empty services.
     */
    if (bt_gatt_dm_attr_cnt(dm) > 1) {
        LOG_INF("The discovery procedure for GATT Service succeeded");

        bt_gatt_dm_data_print(dm);

        err = bt_gattp_handles_assign(dm, gattp);
        if (err) {
            LOG_DBG("Could not init GATT Service client object, error: %d", err);
        } else {
            enable_gattp_indications(gattp);
        }
    } else {
        LOG_WRN("GATT Service could not be found during the discovery");
    }

    err = bt_gatt_dm_data_release(dm);
    if (err) {
        LOG_ERR("Could not release the discovery data, error code: %d", err);
    }

    discover_ancs_first(conn);
}

static void discover_gattp_not_found_cb(struct bt_conn *conn, void *ctx)
{
    LOG_DBG("GATT Service could not be found during the discovery");

    discover_ancs_first(conn);
}

static void discover_gattp_error_found_cb(struct bt_conn *conn, int err, void *ctx)
{
    LOG_ERR("The discovery procedure for GATT Service failed, err %d", err);

    discover_ancs_first(conn);
}

static const struct bt_gatt_dm_cb discover_gattp_cb = {
    .completed = discover_gattp_completed_cb,
    .service_not_found = discover_gattp_not_found_cb,
    .error_found = discover_gattp_error_found_cb,
};

static void discover_gattp(struct bt_conn *conn)
{
    int err = bt_gatt_dm_start(conn, BT_UUID_GATT, &discover_gattp_cb, &gattp);
    if (err) {

        // Only one DM discovery can happen at a time, AMS may be running, so queue it
        if (err == -EALREADY) {
            k_work_schedule(&gatt_discover_retry, K_MSEC(500));
            return;
        }

        LOG_ERR("Failed to start discovery for GATT Service (err %d)", err);
    }
}

static void gatt_discover_retry_handle(struct k_work *item)
{
    discover_gattp(current_conn);
}

static void discover_ancs(struct bt_conn *conn, bool retry)
{
    int err;

    /* 1 Service Discovery at a time */
    if (atomic_test_and_set_bit(&discovery_flags, DISCOVERY_ANCS_ONGOING)) {
        return;
    }

    /* If ANCS is found, do not discover again. */
    if (atomic_test_bit(&discovery_flags, DISCOVERY_ANCS_SUCCEEDED)) {
        atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_ONGOING);
        return;
    }

    /* Check that Service Changed indication is received before discovering ANCS again. */
    if (retry) {
        if (!atomic_test_and_clear_bit(&discovery_flags, SERVICE_CHANGED_INDICATED)) {
            atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_ONGOING);
            return;
        }
    }

    err = bt_gatt_dm_start(conn, BT_UUID_ANCS, &discover_ancs_cb, &ancs_c);
    if (err) {
        LOG_ERR("Failed to start discovery for ANCS (err %d)", err);
        atomic_clear_bit(&discovery_flags, DISCOVERY_ANCS_ONGOING);
    }
}

static void discover_ancs_first(struct bt_conn *conn)
{
    discover_ancs(conn, false);
}

static void discover_ancs_again(struct bt_conn *conn)
{
    discover_ancs(conn, true);
}

static void security_changed(struct bt_conn *conn, bt_security_t level,
                             enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        LOG_INF("Security changed: %s level %u", addr, level);

        if (bt_conn_get_security(conn) >= BT_SECURITY_L2) {
            discovery_flags = ATOMIC_INIT(0);
            discover_gattp(conn);
        }
    } else {
        LOG_ERR("Security failed: %s level %u err %d", addr, level, err);
    }
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection failed (err 0x%02x)", err);
        return;
    }

    current_conn = bt_conn_ref(conn);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    if (current_conn) {
        bt_conn_unref(current_conn);
    }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .security_changed = security_changed,
    .disconnected = disconnected
};

/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] attr Pointer to an iOS App attribute.
 */
static void app_attr_print(const struct bt_ancs_attr *attr)
{
    if (attr->attr_len != 0) {
        LOG_DBG("%s: %s", lit_appid[attr->attr_id],
                (char *)attr->attr_data);
    } else if (attr->attr_len == 0) {
        LOG_DBG("%s: (N/A)", lit_appid[attr->attr_id]);
    }
}

/**@brief Function for printing out errors that originated from the Notification Provider (iOS).
 *
 * @param[in] err_code_np Error code received from NP.
 */
static void err_code_print(uint8_t err_code_np)
{
    switch (err_code_np) {
        case BT_ATT_ERR_ANCS_NP_UNKNOWN_COMMAND:
            LOG_DBG("Error: Command ID was not recognized by the Notification Provider.");
            break;

        case BT_ATT_ERR_ANCS_NP_INVALID_COMMAND:
            LOG_DBG("Error: Command failed to be parsed on the Notification Provider.");
            break;

        case BT_ATT_ERR_ANCS_NP_INVALID_PARAMETER:
            LOG_DBG("Error: Parameter does not refer to an existing object on the Notification Provider.");
            break;

        case BT_ATT_ERR_ANCS_NP_ACTION_FAILED:
            LOG_DBG("Error: Perform Notification Action Failed on the Notification Provider.");
            break;

        default:
            break;
    }
}

/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] attr Pointer to an iOS notification attribute.
 */
static void notif_attr_print(const struct bt_ancs_attr *attr)
{
    if (attr->attr_len != 0) {
        LOG_DBG("%s: %s", lit_attrid[attr->attr_id],
                (char *)attr->attr_data);
    } else if (attr->attr_len == 0) {
        LOG_DBG("%s: (N/A)", lit_attrid[attr->attr_id]);
    }
}

static int parse_notify(const struct bt_ancs_attr *attr)
{
    static struct ble_data_event evt = { 0 };

    switch (attr->attr_id) {
        case ATTR_ID_TITLE:
            evt.data.data.notify.title = attr->attr_data;
            evt.data.data.notify.title_len = attr->attr_len;
            break;

        case ATTR_ID_MESSAGE:
            evt.data.data.notify.body = (char *)attr->attr_data;
            evt.data.data.notify.body_len = attr->attr_len;
            break;

        case ATTR_ID_APP_ID:
            // This comes as example com.facebook.Messenger
            evt.data.data.notify.src = strrchr(attr->attr_data, '.') + 1;
            evt.data.data.notify.src_len = strlen(evt.data.data.notify.src);
            evt.data.data.notify.id = notification_latest.notif_uid;
            break;

        // the last message is Negative action label, send only when all data is received;
        case ATTR_ID_NEGATIVE_ACTION_LABEL:

            evt.data.type = BLE_COMM_DATA_TYPE_NOTIFY;

            zbus_chan_pub(&ble_comm_data_chan, &evt, K_MSEC(250));

            memset(&evt, 0, sizeof(evt));
            break;
        case ATTR_ID_DATE:
        case ATTR_ID_MESSAGE_SIZE:
        case ATTR_ID_SUBTITLE:
        case ATTR_ID_POSITIVE_ACTION_LABEL:
        default:
            // @todo
            break;
    }

    return 0;
}

static void bt_ancs_notification_source_handler(struct bt_ancs_client *ancs_c,
                                                int err, const struct bt_ancs_evt_notif *notif)
{
    if (!err) {
        notification_latest = *notif;

        if (notification_latest.evt_id == BT_ANCS_EVENT_ID_NOTIFICATION_REMOVED) {
            struct ble_data_event evt_notif_rem = {
                .data.type = BLE_COMM_DATA_TYPE_NOTIFY_REMOVE,
                .data.data.notify_remove.id = notification_latest.notif_uid,
            };

            LOG_DBG("Remove notification %d", evt_notif_rem.data.data.notify_remove.id);

            zbus_chan_pub(&ble_comm_data_chan, &evt_notif_rem, K_MSEC(250));

            return;
        }

        bt_ancs_request_attrs(ancs_c, &notification_latest, bt_ancs_write_response_handler);
    }
}

static void bt_ancs_data_source_handler(struct bt_ancs_client *ancs_c,
                                        const struct bt_ancs_attr_response *response)
{
    switch (response->command_id) {
        case BT_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES:
            notif_attr_latest = response->attr;
            notif_attr_print(&notif_attr_latest);
            parse_notify(&notif_attr_latest);
            if (response->attr.attr_id ==
                BT_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER) {
                notif_attr_app_id_latest = response->attr;
            }
            break;

        case BT_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES:
            app_attr_print(&response->attr);
            break;

        default:
            /* No implementation needed. */
            break;
    }
}

static void bt_ancs_write_response_handler(struct bt_ancs_client *ancs_c,
                                           uint8_t err)
{
    err_code_print(err);
}

static int gattp_init(void)
{
    return bt_gattp_init(&gattp);
}


int ble_ancs_init(void)
{
    int err = bt_ancs_client_init(&ancs_c);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER,
                                attr_appid, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_app_attr(&ancs_c,
                                    BT_ANCS_APP_ATTR_ID_DISPLAY_NAME,
                                    attr_disp_name, sizeof(attr_disp_name));
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_TITLE,
                                attr_title, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_MESSAGE,
                                attr_message, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_SUBTITLE,
                                attr_subtitle, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE,
                                attr_message_size, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_DATE,
                                attr_date, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL,
                                attr_posaction, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = bt_ancs_register_attr(&ancs_c,
                                BT_ANCS_NOTIF_ATTR_ID_NEGATIVE_ACTION_LABEL,
                                attr_negaction, BT_ANCS_ATTR_DATA_MAX);
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    err = gattp_init();
    if (err) {
        LOG_ERR("Failed to start ANCS: 0x%x", err);
        return err;
    }

    LOG_INF("Started Apple Notification Center Service client");

    return err;
}
