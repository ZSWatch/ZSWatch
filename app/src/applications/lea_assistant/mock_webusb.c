#include "mock_webusb.h"
#include "message_handler.h"
#include "lea_assistant_app.h"

#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

LOG_MODULE_REGISTER(mock_webusb, CONFIG_ZSW_LEA_ASSISTANT_APP_LOG_LEVEL);

struct webusb_ltv_data {
    uint8_t adv_sid;
    uint16_t pa_interval;
    uint32_t broadcast_id;
    bt_addr_le_t addr;
    char bt_name[BT_NAME_LEN];
    char broadcast_name[BT_NAME_LEN];
} __packed;

static bool data_found(struct bt_data *data, void *user_data)
{
    struct webusb_ltv_data *_parsed = (struct webusb_ltv_data *)user_data;

    LOG_DBG("Found LTV structure with type %u", data->type);

    switch (data->type) {
        case BT_DATA_SID:
            _parsed->adv_sid = data->data[0];
            LOG_DBG("BT_DATA_SID: 0x%X", _parsed->adv_sid);
            return true;
        case BT_DATA_PA_INTERVAL:
            _parsed->pa_interval = sys_get_le16(data->data);
            LOG_DBG("BT_DATA_PA_INTERVAL: 0x%X", _parsed->pa_interval);
            return true;
        case BT_DATA_BROADCAST_ID:
            _parsed->broadcast_id = sys_get_le32(data->data);
            LOG_DBG("BT_DATA_BROADCAST_ID: 0x%X", _parsed->broadcast_id);
            return true;
        case BT_DATA_RPA:
        case BT_DATA_IDENTITY:
            char addr_str[BT_ADDR_LE_STR_LEN];
            _parsed->addr.type = data->data[0];
            memcpy(&_parsed->addr.a, &data->data[1], sizeof(bt_addr_t));
            bt_addr_le_to_str(&_parsed->addr, addr_str, sizeof(addr_str));
            LOG_DBG("Addr: %s", addr_str);
            return true;
        case BT_DATA_NAME_SHORTENED:
        case BT_DATA_NAME_COMPLETE:
            memcpy(_parsed->bt_name, data->data, MIN(data->data_len, BT_NAME_LEN - 1));
            LOG_DBG("BT name: %s", _parsed->bt_name);
            return true;
        case BT_DATA_BROADCAST_NAME:
            memcpy(_parsed->broadcast_name, data->data, MIN(data->data_len, BT_NAME_LEN - 1));
            LOG_DBG("Broadcast name: %s", _parsed->broadcast_name);
            return true;
        default:
            LOG_DBG("Unknown type");
            return true;
    }

    return false;
}

int webusb_transmit(struct net_buf *tx_net_buf)
{
    if (tx_net_buf == NULL) {
        LOG_ERR("Null msg_ptr");
        return 1;
    }
    struct webusb_ltv_data parsed_ltv_data = {0};
    struct webusb_message *webusb_message = (struct webusb_message *)tx_net_buf->b.data;

    struct net_buf_simple msg_net_buf = {
        .data = webusb_message->payload,
        .len = webusb_message->length,
        .size = CONFIG_TX_MSG_MAX_PAYLOAD_LEN,
        .__buf = webusb_message->payload,
    };

    bt_data_parse(&msg_net_buf, data_found, (void *)&parsed_ltv_data);

    switch (webusb_message->sub_type) {
        case MESSAGE_SUBTYPE_SOURCE_FOUND:
            LOG_DBG("MESSAGE_SUBTYPE_SOURCE_FOUND");
            lea_assistant_device_t source = {
                .pa_interval = parsed_ltv_data.pa_interval,
                .sid = parsed_ltv_data.adv_sid,
                .broadcast_id = parsed_ltv_data.broadcast_id,
            };

            bt_addr_le_copy(&source.addr, &parsed_ltv_data.addr);

            if (parsed_ltv_data.broadcast_name[0] != '\0') {
                strcpy(source.name, parsed_ltv_data.broadcast_name);
            } else if (parsed_ltv_data.bt_name[0] != '\0') {
                strcpy(source.name, parsed_ltv_data.bt_name);
            } else {
                // No name
                break;
            }

            lea_assistant_app_add_source_entry(&source);
            break;

        case MESSAGE_SUBTYPE_SINK_FOUND:
            LOG_DBG("MESSAGE_SUBTYPE_SINK_FOUND");
            lea_assistant_device_t sink;

            if (parsed_ltv_data.bt_name[0] != '\0') {
                strcpy(sink.name, parsed_ltv_data.bt_name);
                bt_addr_le_copy(&sink.addr, &parsed_ltv_data.addr);
            } else {
                // No name
                break;
            }

            lea_assistant_app_add_sink_entry(&sink);
            break;

        case MESSAGE_SUBTYPE_SINK_CONNECTED:
            LOG_DBG("MESSAGE_SUBTYPE_SINK_CONNECTED");
            lea_assistant_app_sink_connected();
            break;

        case MESSAGE_SUBTYPE_SINK_DISCONNECTED:
            LOG_DBG("MESSAGE_SUBTYPE_SINK_DISCONNECTED");
            break;

        case MESSAGE_SUBTYPE_SOURCE_ADDED:
            LOG_DBG("MESSAGE_SUBTYPE_SOURCE_ADDED");
            break;

        default:
            break;
    }

    net_buf_unref(tx_net_buf);

    return 0;
}
