#include <ble_comm.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <ble_transport.h>
#include <events/ble_data_event.h>
#include <zephyr/zbus/zbus.h>
#include <zsw_popup_window.h>

LOG_MODULE_REGISTER(ble_comm, LOG_LEVEL_DBG);

#define BLE_COMM_LONG_INT_MIN_MS   (400 / 1.25)
#define BLE_COMM_LONG_INT_MAX_MS   (500 / 1.25)

#define BLE_COMM_CONN_INT_UPDATE_TIMEOUT_MS    5000

ZBUS_CHAN_DECLARE(ble_comm_data_chan);

typedef enum parse_state {
    WAIT_GB,
    WAIT_END,
    PARSE_STATE_DONE,
} parse_state_t;

static char *extract_value_str(char *key, char *data, int *value_len);
static int parse_data(char *data, int len);
static void parse_time(char *data);
static void parse_remote_control(char *data, int len);
static void send_ble_data_event(ble_comm_cb_data_t *data);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);
static void param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
static void update_conn_interval_handler(struct k_work *item);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
    .le_param_updated = param_updated,
};

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE,
                  (CONFIG_BT_DEVICE_APPEARANCE >> 0) & 0xff,
                  (CONFIG_BT_DEVICE_APPEARANCE >> 8) & 0xff),
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
                  BT_UUID_16_ENCODE(BT_UUID_DIS_VAL))
};

static const struct bt_data ad_nus[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BLE_UUID_TRANSPORT_VAL),
};

K_WORK_DELAYABLE_DEFINE(conn_interval_work, update_conn_interval_handler);

static struct bt_conn *current_conn;
static uint32_t max_send_len;
static uint8_t receive_buf[300];
static uint8_t num_parsed_brackets;
static uint8_t parsed_data_index = 0;
static parse_state_t parse_state = WAIT_GB;

static on_data_cb_t data_parsed_cb;

static int pairing_enabled;

static struct ble_transport_cb ble_transport_callbacks = {
    .data_receive = bt_receive_cb,
};

static void auth_cancel(struct bt_conn *conn)
{
    LOG_ERR("Pairing cancelled\n");
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    LOG_DBG("Pairing Complete\n");
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];

    if (bt_conn_get_info(conn, &info) < 0) {
        addr[0] = '\0';
    }

    bt_addr_le_to_str(info.le.remote, addr, sizeof(addr));
    zsw_popup_show("Pairing successful", addr, NULL, 5);
    ble_comm_set_pairable(false);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];

    if (bt_conn_get_info(conn, &info) < 0) {
        addr[0] = '\0';
    }

    bt_addr_le_to_str(info.le.remote, addr, sizeof(addr));
    if (pairing_enabled) {
        zsw_popup_show("Pairing Failed", "Address:", NULL, 5);
    }
    LOG_WRN("Pairing Failed (%d). Disconnecting.\n", reason);
    bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static void pairing_deny(struct bt_conn *conn)
{
    LOG_ERR("Pairing deny\n");
    bt_conn_auth_cancel(conn);
}

static void pairing_accept(struct bt_conn *conn)
{
    LOG_WRN("Pairing accept\n");
    bt_conn_auth_pairing_confirm(conn);
}

static struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = NULL,
    .passkey_entry = NULL,
    .pairing_confirm = pairing_deny,
    .cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb auth_cb_info = {
    .pairing_complete = pairing_complete,
    .pairing_failed = pairing_failed,
};

int ble_comm_init(on_data_cb_t data_cb)
{
    bt_conn_auth_cb_register(&auth_cb_display);
    bt_conn_auth_info_cb_register(&auth_cb_info);

    ble_comm_set_pairable(false);

    int err = ble_transport_init(&ble_transport_callbacks);
    if (err) {
        LOG_ERR("Failed to initialize UART service (err: %d)", err);
        return err;
    }
    data_parsed_cb = data_cb;

    struct bt_le_adv_param adv_param = {
        .options = BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME,
        .interval_min = BT_GAP_ADV_SLOW_INT_MIN,
        .interval_max = BT_GAP_ADV_SLOW_INT_MAX,
    };

    err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), ad_nus, ARRAY_SIZE(ad_nus));
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)\n", err);
    } else {
        LOG_DBG("Advertising successfully started\n");
    }

    return err;
}

int ble_comm_send(uint8_t *data, uint16_t len)
{
    if (len > max_send_len) {
        return -EMSGSIZE;
    }
    return ble_transport_send(current_conn, data, len);
}

void ble_comm_set_pairable(bool pairable)
{
    if (pairable) {
        LOG_WRN("Enable Pairable");
        auth_cb_display.pairing_confirm = pairing_accept;
        bt_conn_auth_cb_register(&auth_cb_display);
    } else {
        LOG_WRN("Disable Pairable\n");
        auth_cb_display.pairing_confirm = pairing_deny;
        bt_conn_auth_cb_register(&auth_cb_display);
    }
    pairing_enabled = pairable;
}

int ble_comm_short_connection_interval(void)
{
    int err;
    struct bt_le_conn_param param = {
        .interval_min = CONFIG_BT_PERIPHERAL_PREF_MIN_INT,
        .interval_max = CONFIG_BT_PERIPHERAL_PREF_MAX_INT,
        .latency = CONFIG_BT_PERIPHERAL_PREF_LATENCY,
        .timeout = CONFIG_BT_PERIPHERAL_PREF_TIMEOUT,
    };

    // If someone explicitly requested short connection interval,
    // don't change it back.
    k_work_cancel_delayable(&conn_interval_work);

    LOG_DBG("Set short conection interval");

    err = bt_conn_le_param_update(current_conn, &param);
    if (err && err != -EALREADY) {
        LOG_WRN("bt_conn_le_param_update failed: %d", err);
    }

    return err;
}

int ble_comm_long_connection_interval(void)
{
    int err;
    struct bt_le_conn_param param = {
        .interval_min = BLE_COMM_LONG_INT_MIN_MS,
        .interval_max = BLE_COMM_LONG_INT_MAX_MS,
        .latency = CONFIG_BT_PERIPHERAL_PREF_LATENCY,
        .timeout = 500,
    };

    LOG_DBG("Set long conection interval");

    err = bt_conn_le_param_update(current_conn, &param);
    if (err && err != -EALREADY) {
        LOG_WRN("bt_conn_le_param_update failed %d", err);
    }

    return err;
}

static void update_conn_interval_handler(struct k_work *item)
{
    LOG_DBG("Change to long connection interval");
    ble_comm_long_connection_interval();
}

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
    if (!err) {
        LOG_INF("MTU exchange done. %d", bt_gatt_get_mtu(current_conn) - 3);

        max_send_len = bt_gatt_get_mtu(current_conn) - 3;
    } else {
        LOG_WRN("MTU exchange failed (err %" PRIu8 ")", err);
    }
}
static void request_mtu_exchange(void)
{
    int err;
    static struct bt_gatt_exchange_params exchange_params;
    exchange_params.func = mtu_exchange_cb;

    err = bt_gatt_exchange_mtu(current_conn, &exchange_params);
    if (err) {
        LOG_WRN("MTU exchange failed (err %d)", err);
    } else {
        LOG_INF("MTU exchange pending");
    }
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (err) {
        LOG_ERR("Connection failed (err %u)", err);
        return;
    }
    current_conn = bt_conn_ref(conn);
    max_send_len = 20;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Connected %s", addr);
    request_mtu_exchange();
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    LOG_INF("Interval: %d, latency: %d, timeout: %d", info.le.interval, info.le.latency, info.le.timeout);

    // Right after a new connection we want short connection interval
    // to let the peer discover services etc. quickly.
    // After some time assume the peer is done and change to longer intervals
    // to save power.
    k_work_schedule(&conn_interval_work, K_MSEC(BLE_COMM_CONN_INT_UPDATE_TIMEOUT_MS));

    if (pairing_enabled) {
        int rc = bt_conn_set_security(conn, BT_SECURITY_L2);
        if (rc != 0) {
            LOG_ERR("Failed to set security: %d", rc);
            bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
        }
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Disconnected: %s (reason %u)", addr, reason);

    if (current_conn) {
        k_work_cancel_delayable(&conn_interval_work);
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }
}

static void param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    LOG_INF("Updated => Interval: %d, latency: %d, timeout: %d", interval, latency, timeout);
}

static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
    LOG_HEXDUMP_DBG(data, len, "RX");

    if (strncmp("Control:", data, MIN(strlen("Control:"), len)) == 0) {
        char *time_start = strstr(data, "Control:");
        return parse_remote_control(time_start + strlen("Control:"), len - strlen("Control:"));
    }

    char *gb_start = strstr(data, "GB(");
    if (gb_start && parse_state != WAIT_GB) {
        LOG_ERR("Parsing error, was waiting end, but got GB");
        parse_state = WAIT_GB;
    }

    char *time_start = strstr(data, "setTime(");
    if (time_start && parse_state == WAIT_GB) {
        time_start += strlen("setTime(");
        parse_time(time_start);
        return;
    }
    switch (parse_state) {
        case WAIT_GB: {
            if (gb_start) {
                gb_start += 3;
                parse_state = WAIT_END;
                num_parsed_brackets = 0;
                parsed_data_index = 0;
                memset(receive_buf, 0, sizeof(receive_buf));
                uint32_t index = gb_start - (char *)data;
                for (int i = index; i < len; i++) {
                    receive_buf[parsed_data_index] = data[i];
                    parsed_data_index++;
                    if (data[i] == '{') {
                        num_parsed_brackets++;
                    } else if (data[i] == '}') {
                        num_parsed_brackets--;
                        if (num_parsed_brackets == 0) {
                            parse_state = PARSE_STATE_DONE;
                            break;
                        }
                    }
                }
            }
            break;
        }
        case WAIT_END: {
            for (int i = 0; i < len; i++) {
                receive_buf[parsed_data_index] = data[i];
                parsed_data_index++;
                if (data[i] == '{') {
                    num_parsed_brackets++;
                } else if (data[i] == '}') {
                    num_parsed_brackets--;
                    if (num_parsed_brackets == 0) {
                        parse_state = PARSE_STATE_DONE;
                        break;
                    }
                }
            }
            break;
        }
        case PARSE_STATE_DONE: {
            LOG_WRN("Busy parsing, ingnoring...");
            break;
        }
        default:
            LOG_WRN("Unhandled state");
            break;
    }
    if (parse_state == PARSE_STATE_DONE) {
        parse_state = WAIT_GB;
        LOG_DBG("%s", receive_buf);
        parse_data(receive_buf, parsed_data_index);
    }
}

static void parse_time(char *start_time)
{
    char *end_time;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    end_time = strstr(start_time, ")");
    if (end_time) {
        errno = 0;
        cb.data.notify.id = strtol(start_time, &end_time, 10);
        if (start_time != end_time && errno == 0) {
            cb.type = BLE_COMM_DATA_TYPE_SET_TIME;
            send_ble_data_event(&cb);
        } else {
            LOG_WRN("Failed parsing time");
        }
    }
}

static char *extract_value_str(char *key, char *data, int *value_len)
{
    char *start;
    char *str = strstr(data, key);
    *value_len = 0;
    if (str == NULL) {
        return NULL;
    }
    str += strlen(key);
    if (*str != '\"') {
        return NULL; // Seems to be an INT?
    }
    str++;
    if (*str == '\0') {
        return NULL; // Got end of data
    }
    start = str;
    str = strstr(str, "\"");
    if (str == NULL) {
        return NULL; // No end of value
    }
    *value_len = str - start;

    return start;
}

static uint32_t extract_value_uint32(char *key, char *data)
{
    char *start;
    char *str = strstr(data, key);
    char *end;
    uint32_t id;

    if (str == NULL) {
        return 0;
    }
    str += strlen(key);
    if (!isdigit((int)*str)) {
        return 0; // No number found
    }
    start = str;
    id = strtol(str, &end, 10);
    // TODO error checking
    return id;
}

static int32_t extract_value_int32(char *key, char *data)
{
    char *start;
    char *str = strstr(data, key);
    char *end;
    int32_t id;

    if (str == NULL) {
        return 0;
    }
    str += strlen(key);
    if (!isdigit((int)*str)) {
        return 0; // No number found
    }
    start = str;
    id = strtol(str, &end, 10);
    // TODO error checking
    return id;
}

static int parse_notify(char *data, int len)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_NOTIFY;
    cb.data.notify.id = extract_value_uint32("id:", data);
    cb.data.notify.src = extract_value_str("src:", data, &cb.data.notify.src_len);
    cb.data.notify.sender = extract_value_str("sender:", data, &cb.data.notify.sender_len);
    cb.data.notify.title = extract_value_str("title:", data, &cb.data.notify.title_len);
    cb.data.notify.body = extract_value_str("body:", data, &cb.data.notify.body_len);

    // Little hack since we know it's JSON, we can terminate all values in the data
    // which saves us some hassle and we can just pass all values null terminated
    // to the callback. Make sure to do it after finish parsing!
    if (cb.data.notify.src_len > 0) {
        cb.data.notify.src[cb.data.notify.src_len] = '\0';
    }
    if (cb.data.notify.sender_len > 0) {
        cb.data.notify.sender[cb.data.notify.sender_len] = '\0';
    }
    if (cb.data.notify.title_len > 0) {
        cb.data.notify.title[cb.data.notify.title_len] = '\0';
    }
    if (cb.data.notify.body_len > 0) {
        cb.data.notify.body[cb.data.notify.body_len] = '\0';
    }

    data_parsed_cb(&cb);

    return 0;
}

static int parse_notify_delete(char *data, int len)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_NOTIFY_REMOVE;
    cb.data.notify.id = extract_value_uint32("id:", data);
    data_parsed_cb(&cb);
    return 0;
}

static int parse_weather(char *data, int len)
{
    //{t:"weather",temp:268,hum:97,code:802,txt:"slightly cloudy",wind:2.0,wdir:14,loc:"MALMO"
    int temp_len;
    char *temp_value;
    float temperature;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_WEATHER;
    int32_t temperature_k = extract_value_uint32("temp:", data);
    cb.data.weather.humidity = extract_value_uint32("hum:", data);
    cb.data.weather.weather_code = extract_value_uint32("code:", data);
    cb.data.weather.wind = extract_value_uint32("wind:", data);
    cb.data.weather.wind_direction = extract_value_uint32("wdir:", data);
    temp_value = extract_value_str("txt:", data, &temp_len);

    strncpy(cb.data.weather.report_text, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));

    // App sends temperature in Kelvin
    temperature = temperature_k - 273.15f;
    cb.data.weather.temperature_c = (int8_t)roundf(temperature);
    send_ble_data_event(&cb);
    return 0;
}

static int parse_musicinfo(char *data, int len)
{
    // {t:"musicinfo",artist:"Ava Max",album:"Heaven & Hell",track:"Sweet but Psycho",dur:187,c:-1,n:-1}
    char *temp_value;
    int temp_len;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_MUSTIC_INFO;
    cb.data.music_info.duration = extract_value_int32("dur:", data);
    cb.data.music_info.track_count = extract_value_int32("c:", data);
    cb.data.music_info.track_num = extract_value_int32("n:", data);
    temp_value = extract_value_str("artist:", data, &temp_len);
    strncpy(cb.data.music_info.artist, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));
    temp_value = extract_value_str("album:", data, &temp_len);
    strncpy(cb.data.music_info.album, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));
    temp_value = extract_value_str("track:", data, &temp_len);
    strncpy(cb.data.music_info.track_name, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));

    send_ble_data_event(&cb);

    return 0;
}

static int parse_musicstate(char *data, int len)
{
    // {t:"musicinfo",artist:"Ava Max",album:"Heaven & Hell",track:"Sweet but Psycho",dur:187,c:-1,n:-1}
    char *temp_value;
    int temp_len;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_MUSTIC_STATE;
    cb.data.music_state.position = extract_value_int32("position:", data);
    cb.data.music_state.shuffle = extract_value_int32("shuffle:", data);
    cb.data.music_state.repeat = extract_value_int32("repeat:", data);

    temp_value = extract_value_str("state:", data, &temp_len);
    if (strncmp(temp_value, "play", temp_len) == 0) {
        cb.data.music_state.playing = true;
    } else {
        cb.data.music_state.playing = false;
    }

    send_ble_data_event(&cb);

    return 0;
}

static int parse_data(char *data, int len)
{
    int type_len;
    char *type;

    type = extract_value_str("t:", data, &type_len);
    if (type == NULL) {
        return -1;
    }

    if (strlen("notify") == type_len && strncmp(type, "notify", type_len) == 0) {
        return parse_notify(data, len);
    }

    if (strlen("notify-") == type_len && strncmp(type, "notify-", type_len) == 0) {
        return parse_notify_delete(data, len);
    }

    if (strlen("weather") == type_len && strncmp(type, "weather", type_len) == 0) {
        return parse_weather(data, len);
    }

    if (strlen("musicinfo") == type_len && strncmp(type, "musicinfo", type_len) == 0) {
        return parse_musicinfo(data, len);
    }

    if (strlen("musicstate") == type_len && strncmp(type, "musicstate", type_len) == 0) {
        return parse_musicstate(data, len);
    }

    return 0;
}

static void parse_remote_control(char *data, int len)
{
    int button;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    button = atoi(data);
    printk("Pressed: %d, len: %d\n", button, len);
    if (button < 0) {
        return;
    }

    cb.type = BLE_COMM_DATA_TYPE_REMOTE_CONTROL;
    cb.data.remote_control.button = button;
    send_ble_data_event(&cb);
}

static void send_ble_data_event(ble_comm_cb_data_t *data)
{
    struct ble_data_event evt;
    memcpy(&evt.data, data, sizeof(ble_comm_cb_data_t));

    zbus_chan_pub(&ble_comm_data_chan, &evt, K_MSEC(250));
}